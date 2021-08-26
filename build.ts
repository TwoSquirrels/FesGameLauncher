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

/*
BUILD FLOW
- TypeScript --[TypeScriptCompile]-> JavaScript
- SCSS --[Sass]-> CSS
- EJS --[EJS(NodeJS)]-> HTML
*/

////////////////////////////////////////////////////////////////////////////////
// imports
////////////////////////////////////////////////////////////////////////////////

import child from "child_process";
import dateFormat from "dateformat";
import ejs from "ejs";
import fs from "fs-extra";
import glob from "glob";
import log4js from "log4js";
import path from "path";
import util from "util";

////////////////////////////////////////////////////////////////////////////////
// constants
////////////////////////////////////////////////////////////////////////////////

const config = JSON.parse(fs.readFileSync("./package.json").toString());

////////////////////////////////////////////////////////////////////////////////
// utils
////////////////////////////////////////////////////////////////////////////////

// for tasks

const hr = (repeat: number, character: string = "—"): string => character.repeat(repeat);

function exec(logger: log4js.Logger, command: string): Promise<void> {
  return new Promise<void>((resolve, reject) => {
    logger.debug("> " + command);
    child.exec(
      command,
      { windowsHide: true },
      (error: child.ExecException | null, stdout: string, stderr: string): void => {
        if (stdout) stdout.slice(0, -1).split("\n").forEach(out => logger.info(out));
        if (stderr) stderr.slice(0, -1).split("\n").forEach(err => logger.error(err));
        if (error) reject(error);
        else resolve();
      }
    );
  })
};

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

////////////////////////////////////////////////////////////////////////////////
// tasks
////////////////////////////////////////////////////////////////////////////////

const tasks = new Map<string, () => Promise<void>>();

tasks.set("build", async () => {

  const logger = log4js.getLogger("BUILD");

  logger.info(hr(40));

  try {

    try {
      if (await fs.pathExists("build/site")) {
        if (!(await fs.stat("build/site")).isDirectory()) throw '"build/site" is not a directory.';
        if ((await fs.promises.readdir("build/site")).length !== 0) throw '"build/site" is not empty';
      }
      if (await fs.pathExists("build/electron")) {
        if (!(await fs.stat("build/electron")).isDirectory()) throw '"build/electron" is not a directory.';
        if ((await fs.promises.readdir("build/electron")).length !== 0) throw '"build/electron" is not empty';
      }
    } catch (err) {
      logger.error(err);
      throw new Error();
    }

    await Promise.all([
      (async () => {

        // TypeScript

        const logger = log4js.getLogger("TYPESCRIPT");

        logger.info("Compiling TypeScript...");
        try {
          const options: string = '--target "ESNEXT" --allowJs --declaration --declarationMap --sourceMap --strict --moduleResolution "node" --resolveJsonModule';
          await Promise.all(
            // site
            (await find("src/site/**/*.{ts,js}")).map(tsFile => (async () => {
              await exec(
                logger,
                `npx tsc --outFile "./build/site/resources/${config.version}/${tsFile.replace(/src\/site/, "").replace(/ts$/g, "js")}" "./${tsFile}" ${options}`
              );
              logger.debug(`Compiled "${tsFile}".`);
            })()).concat(
              // electron
              (await find("src/electron/**/*.{ts,js}")).map(tsFile => (async () => {
                await exec(
                  logger,
                  `npx tsc --outFile "./build/electron/${tsFile.replace(/src\/electron/, "").replace(/ts$/, "js")}" "./${tsFile}" ${options}`
                );
                logger.debug(`Compiled "${tsFile}".`);
              })())
            )
          );
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
          await exec(logger, `npx sass --style=expanded --no-source-map src/site:build/site/resources/${config.version}`);
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
        try {
          config.site.version = config.version;
          for (const ejsFile of await find("src/site/**/*.{ejs,html}")) {
            if (ejsFile === "src/site/template.ejs") continue;
            const page = require("./" + ejsFile.replace(/(ejs|html)$/g, "json"));
            page.path = ejsFile.replace(/src\/site\/|\.(ejs|html)$/g, "");
            await fs.promises.writeFile(
              `build/site/${page.path}.html`,
              await renderEJSFile(
                "src/site/template.ejs",
                {
                  render: {
                    body: ejsFile
                  },
                  site: config.site,
                  libs: config.libs,
                  page: page,
                }
              )
            );
            logger.debug(`Compiled "${ejsFile}".`);
          }
        } catch (err) {
          logger.error(err);
          logger.error("Failed to render EJS.");
          throw new Error();
        }
        logger.info("Completed rendering EJS!");

      })(),
    ]);

    logger.info("Creating a link for the site directory in the electron directory...");
    try {
      await fs.promises.symlink(path.resolve("build/site"), path.resolve("build/electron/site"), "junction");
    } catch (err) {
      logger.error(err);
      logger.error("Failed to create a link.");
      throw new Error();
    }
    logger.info("Completed creating a link!");

    logger.info(hr(40));
    logger.info("BUILD SUCCESS");
    logger.info(hr(40));

    return;

  } catch (err) {

    logger.info(hr(40));
    logger.error("BUILD FAILURE");
    logger.info(hr(40));

    throw new Error();

  }

});

tasks.set("clean", async () => {

  const logger = log4js.getLogger("CLEAN");

  logger.info(hr(40));

  try {

    logger.info('Cleaning directories, "build/site" and "build/electron"...');
    try {
      await fs.emptyDir("build/site");
      await fs.emptyDir("build/electron");
    } catch (err) {
      logger.error(err);
      logger.error('Failed to clean directories.');
      throw new Error();
    }
    logger.info('Completed cleaning directories!');

    logger.info(hr(40));
    logger.info("CLEAN SUCCESS");
    logger.info(hr(40));

  } catch (err) {

    logger.info(hr(40));
    logger.error("CLEAN FAILURE");
    logger.info(hr(40));

    throw new Error();

  }

});

tasks.set("cleanlog", async () => {

  const logger = log4js.getLogger("CLEANLOG");

  logger.info(hr(40));

  try {

    logger.info('Cleaning "logs" directory...');
    try {
      await fs.emptyDir("logs");
    } catch (err) {
      logger.error(err);
      logger.error("Failed to clean directory.");
      throw new Error();
    }
    logger.info("Completed cleaning directory!");

    logger.info(hr(40));
    logger.info("CLEANLOG SUCCESS");
    logger.info(hr(40));

  } catch (err) {

    logger.info(hr(40));
    logger.error("CLEANLOG FAILURE");
    logger.info(hr(40));

    throw new Error();

  }

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

  try {

    for (let iThTask = 2; iThTask < process.argv.length; ++iThTask) {

      // A task

      const taskName = process.argv[iThTask];

      console.log("");
      console.log(`Task: [${taskName}]`);
      console.log("");

      const task = tasks.get(taskName);
      if (task === undefined) {

        console.error(`There is no Task [${taskName}]`);
        throw new Error();

      } else {

        // set log config
        await fs.mkdirs("logs/");
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
              filename: `logs/${taskName}-${dateFormat(new Date(), "yyyy_mm_dd-HH_MM_ss_L")}.log`,
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
        await task();

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
