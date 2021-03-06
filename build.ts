/*!
 * ProjectName: fesgamelauncher
 * FileName: build.ts
 * Encoding: UTF-8
 * Author: TwoSquirrels
 * CreationDate: Aug 06, 2021
 * -----------------------------------------------------------------------------
 * Copyright 2021 APC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

////////////////////////////////////////////////////////////////////////////////
// imports
////////////////////////////////////////////////////////////////////////////////

import child from "child_process";
import dateFormat from "dateformat";
import ejs from "ejs";
import express from "express";
import fs from "fs-extra";
import glob from "glob";
import http from "http";
import log4js from "log4js";
import opener from "opener";
import path from "path";
import terser from "terser";
import util from "util";

////////////////////////////////////////////////////////////////////////////////
// constants
////////////////////////////////////////////////////////////////////////////////

const config = fs.readJsonSync("package.json");

////////////////////////////////////////////////////////////////////////////////
// utils
////////////////////////////////////////////////////////////////////////////////

// for tasks

// 罫線を返すだけ
const hr = (repeat: number, character: string = "—"): string =>
  character.repeat(repeat);

// ロギングしながら子プロセスでコマンドを叩く
function exec(logger: log4js.Logger, command: string): Promise<void> {
  return new Promise<void>((resolve, reject) => {
    logger.debug("> " + command);
    child.exec(
      command,
      { windowsHide: true },
      (
        error: child.ExecException | null,
        stdout: string,
        stderr: string
      ): void => {
        // 出力を改行ごとに分けてロギング
        if (stdout)
          stdout
            .slice(0, -1)
            .split("\n")
            .forEach((out) => logger.info(out));
        if (stderr)
          stderr
            .slice(0, -1)
            .split("\n")
            .forEach((err) => logger.error(err));
        // 結果を返す
        if (error) reject(error);
        else resolve();
      }
    );
  });
}

// press any key to continue
function waitkey(): Promise<void> {
  process.stdin.setRawMode(true);
  return new Promise<void>((resolve) =>
    process.stdin.once("data", () => {
      process.stdin.setRawMode(false);
      resolve();
    })
  );
}

// promisify
function renderEJSFile(path: string, data: ejs.Data): Promise<string> {
  return new Promise<string>((resolve, reject) => {
    ejs.renderFile(path, data, (err, str: string) => {
      if (err) reject(err);
      else resolve(str);
    });
  });
}
const find = util.promisify(glob);
const startServer = (
  app: express.Express,
  port: number
): Promise<http.Server> =>
  new Promise((resolve) => {
    let server: http.Server | undefined;
    server = app.listen(port, () => resolve(server!));
  });

////////////////////////////////////////////////////////////////////////////////
// tasks
////////////////////////////////////////////////////////////////////////////////

const tasks = new Map<string, (logger: log4js.Logger) => Promise<void>>();

// 出力先が空いているかを確認してから、
// ライブラリとTypeScriptとSassとEJSを並行してビルド、
// electronからsiteを、site/resourcesからlibsを参照できるようシンボリックリンクを作成
tasks.set("BUILD", async (logger) => {
  try {
    // "build/site" があればそれが空のディレクトリかを確認、なければ作成
    if (await fs.pathExists("build/site")) {
      if (!(await fs.stat("build/site")).isDirectory())
        throw '"build/site" is not a directory.';
      if ((await fs.promises.readdir("build/site")).length !== 0)
        throw '"build/site" is not empty';
    } else await fs.mkdirp("build/site");
    // "build/electron" があればそれが空のディレクトリかを確認、なければ作成
    if (await fs.pathExists("build/electron")) {
      if (!(await fs.stat("build/electron")).isDirectory())
        throw '"build/electron" is not a directory.';
      if ((await fs.promises.readdir("build/electron")).length !== 0)
        throw '"build/electron" is not empty';
    } else await fs.mkdirp("build/electron");
  } catch (err) {
    logger.error(err);
    throw new Error();
  }

  // ライブラリとリソースとTypeScriptとSassとEJSを同時にビルド
  await Promise.all([
    (async () => {
      // Libraries

      const logger = log4js.getLogger("LIBRARIES");

      logger.info("Updating Libraries...");
      try {
        // 新しいライブラリの追加と古いライブラリの除去を同時にする
        await Promise.all([
          // remove old libraries
          ...(
            await find("build/libs/*")
          ).map(async (libDir) => {
            if (
              config.libs.some(
                (lib: any) => libDir.replace(/build\/libs\//, "") === lib.name
              )
            )
              return;
            await fs.remove(libDir);
            logger.info(`Removed "${libDir}".`);
          }),
          // copy new libraries
          ...config.libs.map((lib: any) =>
            (async () => {
              if (await fs.pathExists(`build/libs/${lib.name}`)) {
                logger.info(`"${lib.name}" already exists.`);
                return;
              }
              logger.info(`Copying "${lib.name}"...`);
              try {
                for (const item of lib.requires) {
                  switch (typeof item) {
                    // ファイル/ディレクトリをそのまま
                    case "string":
                      for (const file of await find(`libs/${lib.name}/${item}`))
                        await fs.copy(file, `build/${file}`);
                      logger.debug(`Copied "libs/${lib.name}/${item}".`);
                      break;
                    // ファイルを結合/圧縮
                    case "object":
                      // concat
                      let str: string = "";
                      for (const file of item.files)
                        for (const f of await find(`libs/${lib.name}/${file}`, {
                          nodir: true,
                        }))
                          str +=
                            (str === "" ? "" : "\n") +
                            (await fs.promises.readFile(f, "utf-8"));
                      await fs.outputFile(
                        `build/libs/${lib.name}/${item.name}`,
                        str,
                        "utf-8"
                      );
                      logger.debug(
                        `Concatenated ${JSON.stringify(
                          item.files.map(
                            (file: string) => `libs/${lib.name}/${file}`
                          )
                        )} to "build/libs/${lib.name}/${item.name}".`
                      );
                      // minify
                      switch (item.minify) {
                        case "css":
                          await exec(
                            logger,
                            `npx sass --style=compressed --no-source-map build/libs/${lib.name}/${item.name}:build/libs/${lib.name}/${item.name}`
                          );
                          logger.debug(
                            `Minified "build/libs/${lib.name}/${item.name}" with sass.`
                          );
                          break;
                        case "js":
                          // minify js
                          await fs.outputFile(
                            `build/libs/${lib.name}/${item.name}`,
                            (
                              await terser.minify(
                                await fs.promises.readFile(
                                  `build/libs/${lib.name}/${item.name}`,
                                  "utf-8"
                                )
                              )
                            ).code,
                            "utf-8"
                          );
                          logger.debug(
                            `Minified "build/libs/${lib.name}/${item.name}" with terser.`
                          );
                          break;
                      }
                      break;
                  }
                }
              } catch (err) {
                logger.error(`Failed to copy "${lib.name}".`);
                throw new Error();
              }
              logger.info(`Completed copying "${lib.name}"!`);
            })()
          ),
        ]);
      } catch (err) {
        logger.error("Failed to update libraries.");
        throw new Error();
      }
      logger.info("Completed updating libraries!");
    })(),
    (async () => {
      // Resources

      const logger = log4js.getLogger("RESOURCES");

      logger.info("Copying resources...");
      try {
        await Promise.all([
          // site
          ...(
            await find("resources/site/**/*", { nodir: true })
          ).map(async (resource) => {
            // コピー！
            await fs.copy(
              resource,
              resource.replace(/resources\/site/, `build/site/resources`)
            );
            logger.debug(`Copied "${resource}".`);
          }),
          // electron
          ...(
            await find("resources/electron/**/*", { nodir: true })
          ).map(async (resource) => {
            // コピー！
            await fs.copy(
              resource,
              resource.replace(/resources\/electron/, "build/electron")
            );
            logger.debug(`Copied "${resource}".`);
          }),
        ]);
      } catch (err) {
        logger.error("Failed to copy resources.");
        throw new Error();
      }
      logger.info("Completed copying resources!");
    })(),
    (async () => {
      // TypeScript

      const logger = log4js.getLogger("TYPESCRIPT");

      logger.info("Compiling TypeScript...");
      try {
        const options: string =
          '--target "ESNEXT" --allowJs --declaration --declarationMap --sourceMap --strict --moduleResolution "node" --allowSyntheticDefaultImports';
        // siteディレクトリとelectronディレクトリを同時にコンパイル
        await Promise.all([
          // site
          ...(
            await find("src/site/**/*.{ts,js}", { nodir: true })
          ).map(async (tsFile) => {
            await exec(
              logger,
              `npx tsc ${options} --outDir "./build/site/resources/js/${path.dirname(
                tsFile.replace(/src\/site/, "")
              )}" "./${tsFile}" --module "umd"`
            );
            logger.debug(`Compiled "${tsFile}".`);
          }),
          // electron
          ...(
            await find("src/electron/**/*.{ts,js}", { nodir: true })
          ).map(async (tsFile) => {
            await exec(
              logger,
              `npx tsc ${options} --outDir "./build/electron/${path.dirname(
                tsFile.replace(/src\/electron/, "")
              )}" "./${tsFile}" --module "commonjs"`
            );
            logger.debug(`Compiled "${tsFile}".`);
          }),
        ]);
      } catch (err) {
        logger.error("Failed to compile TypeScript.");
        throw new Error();
      }
      logger.info("Completed compiling TypeScript!");
    })(),
    (async () => {
      // Sass

      const logger = log4js.getLogger("SASS");

      logger.info("Compiling Sass...");
      try {
        await exec(
          logger,
          `npx sass --style=expanded src/site:build/site/resources/css`
        );
      } catch (err) {
        logger.error("Failed to compile Sass.");
        throw new Error();
      }
      logger.info("Completed compiling Sass!");
    })(),
    (async () => {
      // EJS

      const logger = log4js.getLogger("EJS");

      logger.info("Rendering EJS...");
      let nowFile: string | undefined;
      try {
        config.site.version = config.version;
        // ejsData
        let dataCustomizer: (
          logger: Log4js.Logger,
          data: any
        ) => (Promise<void> | void) | null = (() => {
          let ejsDataModule: unknown;
          try {
            ejsDataModule = require("./tasks/build/ejsData");
            logger.info("Loading ");
          } catch (err) {
            logger.info("");
            return null;
          }
          if (typeof ejsDataModule !== "function")
            throw new Error(
              `tasks/build/ejsData module is not a function.\nIt's a ${typeof ejsDataModule}.`
            );
        })();
        // 各ページ
        for (const ejsFile of await find("src/site/**/*.{ejs,html}", {
          nodir: true,
        })) {
          if (ejsFile === "src/site/template.ejs") continue;
          nowFile = ejsFile;
          // ページ設定を読み込み
          const page = await fs.readJson(
            ejsFile.replace(/(ejs|html)$/g, "json")
          );
          // ページのパスを取得
          page.path = ejsFile.replace(/src\/site\/|\.(ejs|html)$/g, "");
          // ページの独自データを取得
          page.data = null;
          if (dataCustomizer) {
            if (dataCustomizer.constructor.name === "AsyncFunction")
              await dataCustomizer(logger, page);
            else dataCustomizer(logger, page);
          }
          // レンダリングしたEJSをHTMLファイルに書き込み
          await fs.outputFile(
            `build/site/${page.path}.html`,
            // EJSファイルにプロパティを渡しレンダリング
            await renderEJSFile("src/site/template.ejs", {
              render: {
                body: ejsFile,
              },
              site: config.site,
              libs: config.libs,
              page,
            })
          );
          logger.debug(`Compiled "${ejsFile}".`);
        }
      } catch (err) {
        logger.error(`Rendering error in "${nowFile}".`);
        logger.error(err);
        logger.error("Failed to render EJS.");
        throw new Error();
      }
      logger.info("Completed rendering EJS!");
    })(),
  ]);

  // Symbolic Link
  await Promise.all([
    (async () => {
      // build/electron/site/ -> build/site/
      logger.info(
        "Creating a link for the site directory in the electron directory..."
      );
      try {
        await fs.promises.symlink(
          path.resolve("build/site"),
          path.resolve("build/electron/site"),
          "junction"
        );
      } catch (err) {
        logger.error(err);
        logger.error("Failed to create a site link.");
        throw new Error();
      }
      logger.info("Completed creating a site link!");
    })(),
    (async () => {
      // build/site/resources/libs/ -> build/libs/
      logger.info(
        "Creating a link for the libs directory in the site/resources directory..."
      );
      try {
        await fs.promises.symlink(
          path.resolve("build/libs"),
          path.resolve("build/site/libs"),
          "junction"
        );
      } catch (err) {
        logger.error(err);
        logger.error("Failed to create a libs link.");
        throw new Error();
      }
      logger.info("Completed creating a libs link!");
    })(),
  ]);
});

// 出力先(buildディレクトリ)の中身を削除
tasks.set("CLEAN", async (logger) => {
  logger.info('Cleaning "build/site" and "build/electron" directory...');
  try {
    await Promise.all([
      fs.emptyDir("build/site"),
      fs.emptyDir("build/electron"),
    ]);
  } catch (err) {
    logger.error(err);
    logger.error("Failed to clean directories.");
    throw new Error();
  }
  logger.info("Completed cleaning directories!");
});

// ログを全て削除
tasks.set("CLEANLOG", async (logger) => {
  logger.info('Cleaning "logs" directory...');
  try {
    await fs.emptyDir("logs");
  } catch (err) {
    logger.error(err);
    logger.error("Failed to clean directory.");
    throw new Error();
  }
  logger.info("Completed cleaning directory!");
});

// Electronのデバッグ実行
tasks.set("ELECTRON", async (logger) => {
  logger.info("Starting Electron...");
  try {
    await exec(logger, "npx electron build/electron/main.js");
  } catch (err) {
    logger.error(err);
    logger.error("Electron terminated abnormally.");
    throw new Error();
  }
  logger.info("Finished Electron!");
});

// サーバーを建ててブラウザを起動する
tasks.set("SERVER", async (logger) => {
  // サーバーを建てる
  logger.info("Launching a server...");
  const app = express();
  let server: http.Server | undefined;
  try {
    const root = path.resolve("build/site");
    app.use(express.static(root, { extensions: ["html", "htm"] }));
    app.use((_req, res) => res.status(404).sendFile("404.html", { root }));
    server = await startServer(app, 8080);
  } catch (err) {
    logger.error(err);
    logger.error("Failed to launch the server.");
    throw new Error();
  }
  logger.info("Succeeded in launching the server!");

  // ブラウザでlocalhost:8080を開く
  (() => {
    const logger = log4js.getLogger("OPEN");
    logger.info("Opening http://localhost:8080/ ...");
    try {
      opener("http://localhost:8080/");
    } catch (err) {
      logger.error(err);
      logger.error("Failed to open the site.");
      return;
    }
    logger.info("Succeeded in opening the site!");
  })();

  console.log("Press any key to stop the server...");
  await waitkey();
  server!.close();
  logger.info("Stopped the server!");
});

// パッケージング
tasks.set("PACKAGE", async (logger) => {
  logger.info("Packaging a Electron app...");
  try {
    // Cleaning
    logger.info('Cleaning "build/package" and "dist/electron"...');
    await Promise.all([
      fs.emptyDir("build/package"),
      fs.emptyDir("dist/electron"),
    ]);
    logger.info("Completed cleaning!");

    const files = {
      css: Array<string>(),
      js: Array<string>(),
      others: Array<string>(),
    };

    // Selecting
    logger.info("Carefully selecting files to put in the package...");
    for (const file of await find("build/electron/**/*", {
      nodir: true,
      follow: true,
    })) {
      const pkPath = file.replace(/build\/electron\//, "");
      // contain libs?
      if (pkPath.split("/").includes("libs")) {
        files.others.push(pkPath);
        continue;
      }
      switch (path.extname(pkPath)) {
        case ".css":
          files.css.push(pkPath);
          break;
        case ".js":
          files.js.push(pkPath);
          break;
        case ".map":
        case ".ts":
          break;
        default:
          files.others.push(pkPath);
          break;
      }
    }
    logger.info("Completed selecting!");
    logger.info(`Files: ${JSON.stringify(files)}`);

    // Processing
    logger.info("Processing files to put in the package...");
    await Promise.all([
      // minify css with sass
      ...files.css.map(async (css) => {
        try {
          await exec(
            logger,
            `npx sass --style=compressed --no-source-map build/electron/${css}:build/package/${css}`
          );
        } catch (err) {
          logger.error(err);
          logger.error(`Failed to minify "${css}" with sass.`);
          throw new Error();
        }
        logger.info(`Completed minifying "${css}" with sass!`);
      }),
      // minify js with terser
      ...files.js.map(async (js) => {
        try {
          await fs.outputFile(
            `build/package/${js}`,
            (
              await terser.minify(
                await fs.promises.readFile(`build/electron/${js}`, "utf-8")
              )
            ).code,
            "utf-8"
          );
        } catch (err) {
          logger.error(err);
          logger.error(`Failed to minify "${js}" with terser.`);
          throw new Error();
        }
        logger.info(`Completed minifying "${js}" with terser!`);
      }),
      // copy others as it is
      ...files.others.map(async (other) => {
        try {
          await fs.copy(`build/electron/${other}`, `build/package/${other}`);
        } catch (err) {
          logger.error(err);
          logger.error(`Failed to copy "${other}".`);
          throw new Error();
        }
        logger.info(`Completed copying "${other}"!`);
      }),
    ]);
    logger.info("Completed Processing!");

    // Electron Builder
    logger.info("Started to packaging the electron app...");
    await exec(logger, "npx electron-builder");
    logger.info("Finished packaging the electron app!");

    // UserData
    logger.info("Started to copying UserData...");
    try {
      await Promise.all(
        (
          await find("dist/electron/*/*/*.exe", { nodir: true })
        ).map(async (exe) => {
          for (const userData of await find("UserData/*")) {
            if (userData === "UserData/log.txt") continue;
            await fs.copy(
              userData,
              `${path.dirname(exe)}${userData.replace(/UserData/, "")}`
            );
          }
          logger.debug(`Copied UserData to "${path.dirname(exe)}".`);
        })
      );
    } catch (err) {
      logger.error(err);
      logger.error("Failed to copy UserData.");
      throw new Error();
    }
    logger.info("Finished copying UserData!");
  } catch (err) {
    logger.error(err);
    logger.error("Failed to pack the app.");
    throw new Error();
  }
  logger.info("Completed packaging!");
});

// Web公開用
tasks.set("PUBLISH", async (logger) => {
  logger.info("Building a site for publication...");
  try {
  } catch (err) {
    logger.error(err);
    logger.error("Failed to build.");
    throw new Error();
  }
  logger.info("Completed building!");
});

////////////////////////////////////////////////////////////////////////////////
// run tasks
////////////////////////////////////////////////////////////////////////////////

if (process.argv.length <= 2) {
  console.error("No tasks have been set.");
  console.log("");
  process.exit(1);
}

(async () => {
  console.log(hr(80, "="));
  console.log(` START BUILD TASKS (for ${config.name}@${config.version})`);
  console.log(hr(80, "="));

  // load tasks
  (await find("tasks/*.{js,ts}", { nodir: true })).forEach(
    (taskScript: string) => {
      try {
        const task = require(`./${taskScript}`);
        if (typeof task !== "function")
          throw new Error(
            `This module is not a function.\nIt's a ${typeof task}.`
          );
        if (task.constructor.name !== "AsyncFunction")
          throw new Error(
            `This module is not a async function.\nIt's a ${typeof task}.`
          );
        tasks.set(path.parse(taskScript).name.toUpperCase(), task);
      } catch (err) {
        console.log("");
        console.error(`Failed to load "${taskScript}".`);
        console.error(err);
      }
    }
  );

  try {
    for (let iThTask = 2; iThTask < process.argv.length; ++iThTask) {
      // A task

      const taskName = process.argv[iThTask].toUpperCase();

      console.log("");
      console.log(`Task: [${taskName}]`);
      console.log("");

      const task = tasks.get(taskName);
      if (task === undefined) {
        console.error(`There is no Task [${taskName}]`);
        throw new Error();
      } else {
        // set log config
        await fs.mkdirp("logs/");
        log4js.configure({
          appenders: {
            console: {
              type: "stdout",
            },
            consoleFilter: {
              type: "logLevelFilter",
              appender: "console",
              level: "info",
            },
            file: {
              type: "fileSync",
              filename: `logs/${taskName}-${dateFormat(
                new Date(),
                "yyyy_mm_dd-HH_MM_ss_L"
              )}.log`,
            },
          },
          categories: {
            default: {
              appenders: ["consoleFilter", "file"],
              level: "all",
            },
          },
        });

        // run a task
        const logger = log4js.getLogger(taskName);
        logger.info(hr(40));
        try {
          await task(logger);
          logger.info(hr(40));
          logger.info(`${taskName} SUCCESS`);
          logger.info(hr(40));
        } catch (err) {
          logger.info(hr(40));
          logger.error(`${taskName} FAILURE`);
          logger.info(hr(40));
          throw new Error();
        }
      }
    }

    console.log("");
    console.log(hr(80, "="));
    console.log(" COMPLETE BUILD TASKS");
    console.log(hr(80, "="));
    console.log("");

    process.exit(0);
  } catch (err) {
    console.log("");
    console.error(hr(80, "="));
    console.error(" FAILED BUILD TASKS");
    console.error(hr(80, "="));
    console.log("");

    process.exit(2);
  }
})();
