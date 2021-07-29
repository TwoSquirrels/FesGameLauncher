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

// import

import config from "./package.json";
import sh from "shelljs";
import log4js from "log4js";

// constants

const hr: string = "-".repeat(32);

// settings

log4js.configure({
  appenders: {
    console: { type: "stdout", level: "info" },
    file: { type: "dateFile", filename: "logs/build", pattern: "yyyy_MM_dd-hh_mm_ss.log" },
  },
  categories: {
    default: { appenders: ["console", "file"], level: "all" },
  },
});

const loggers = {
  build: log4js.getLogger("BUILD"),
  ts: log4js.getLogger("TYPESCRIPT"),
  sass: log4js.getLogger("SASS"),
  ejs: log4js.getLogger("EJS"),
};

// utils

async function exec(
  logger: log4js.Logger,
  command: string | (() => sh.ShellString),
  messages: { failed?: string, start?: string, success?: string } = {}
): Promise<void> {

  // start message
  await (async () => {
    if (messages.start !== undefined) logger.info(messages.start);
  })();

  // execute
  const result = await (async () => typeof command === "string" ? sh.exec(command, { silent: true }) : command())();

  // stdout & stderr
  await (async () => {
    const std = new Map<string, string>();
    std.set("out", result.stdout);
    std.set("err", result.stderr);
    std.forEach((value, key) => {
      if (value && value.slice(-1) === "\n") std.set(key, value.slice(0, -1));
    });
    if (std.get("out")) logger.info(std.get("out"));
    if (std.get("err")) logger.error(std.get("err"));
  })();

  await (async () => {
    if (result.code === 0) {
      // success message
      if (messages.success !== undefined) logger.info(messages.success);
    } else {
      // failed message
      if (messages.failed !== undefined) {
        logger.error(messages.failed);
        throw new Error(messages.failed);
      } else throw new Error();
    }
  })();

}

// main

(async () => {

  loggers.build.info(hr);
  loggers.build.info(`BUILDING ${config.name} ${config.version}`);
  loggers.build.info(hr);

  try {

    await exec(
      loggers.build,
      () => sh.rm("-rf", "build/*"),
      {
        start: 'Cleaning contents of "build" directory...',
        success: "Completed cleaning.",
        failed: "Failed cleaning.",
      }
    );

    await Promise.all([
      (async () => {

        // TypeScript

        await exec(
          loggers.ts,
          "tsc",
          {
            start: "Compiling TypeScript...",
            success: "Completed compiling TypeScript.",
            failed: "Failed compiling TypeScript.",
          }
        );

      })(),
      (async () => {

        // Sass

        await exec(
          loggers.sass,
          "sass --style=expanded --no-source-map src:build",
          {
            start: "Compiling Sass...",
            success: "Completed compiling Sass.",
            failed: "Failed compiling Sass.",
          }
        );

      })(),
      (async () => {

        // EJS

      })(),
    ]);

    loggers.build.info(hr);
    loggers.build.info("BUILD SUCCESS");
    loggers.build.info(hr);

  } catch (err) {

    loggers.build.error(hr);
    loggers.build.error("BUILD FAILURE");
    loggers.build.error(hr);

  }

  log4js.shutdown((err: any) => {
    if (err) throw err;
    process.exit(0);
  })

})();
