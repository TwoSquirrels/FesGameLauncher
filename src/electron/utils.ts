/*!
 * ProjectName: electron
 * FileName: utils.ts
 * Encoding: UTF-8
 * Author: TwoSquirrels
 * CreationDate: Oct 01, 2021
 * --------------------------------------------------------------------------------
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

/* imports */

import * as electron from "electron";
import * as fs from "fs-extra";
import * as glob from "glob";
import * as log4js from "log4js";
import * as path from "path";
import * as util from "util";

/* variables */

let config: {
  log?: unknown;
} = {};

/* module */

export const find = util.promisify(glob);

export const userDataPath = (...paths: string[]) =>
  path.join(
    electron.app.isPackaged
      ? electron.app.getPath("exe")
      : path.resolve("UserData"),
    ...paths
  );

export async function init(log: unknown): Promise<void> {
  config.log = log;
  log4js.configure({
    appenders: {
      file: {
        type: "fileSync",
        filename: userDataPath("log.txt"),
      },
    },
    categories: {
      default: {
        appenders: config.log ? ["file"] : [],
        level: "all",
      },
    },
  });
  if (config.log) await fs.remove(userDataPath("log.txt"));
}

export const newLogger = (category: string) => log4js.getLogger(category);

export function lang(
  langs: Map<string, string> | object,
  defaultLang: string = "EN"
): string {
  const langsMap =
    langs instanceof Map
      ? langs
      : new Map<string, string>(Object.entries(langs));
  return (
    langsMap.get(
      typeof config.log === "string" ? config.log.toUpperCase() : defaultLang
    ) ??
    langsMap.get(defaultLang) ??
    ""
  );
}
