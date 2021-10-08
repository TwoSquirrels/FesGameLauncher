/*!
 * ProjectName: fesgamelauncher
 * FileName: items.ts
 * Encoding: UTF-8
 * Author: TwoSquirrels
 * CreationDate: Sep 25, 2021
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
import * as path from "path";

import * as utils from "./utils";

/* module */

export function register() {
  const logger = utils.newLogger("ITEMS");

  electron.ipcMain.handle(
    "items.get",
    async (_event, category: "games" | "movies" | "others") => {
      logger.info(
        utils.lang({
          EN: `Fetching items from "${category}"...`,
          JP: `"${category}"のアイテム取得開始`,
        })
      );
      const items: unknown[] = [];
      await Promise.all(
        (
          await utils.find(
            utils.userDataPath(`items/${category}/*/info.json`),
            { nodir: true }
          )
        ).map(async (info) => {
          try {
            const item = await fs.readJson(info);
            if (Object.prototype.toString.call(item) !== "[object Object]")
              throw new Error(
                utils.lang({
                  EN: `"${category}/${item.id}/info.json" is not an Object.`,
                  JP: `"${category}/${item.id}/info.json" が Object ではありません。`,
                })
              );
            item.id = info.split("/")[info.split("/").length - 2];
            item.category = category;
            // Confirm if format is valid
            const missing: { property: string; type: string }[] = [];
            if (typeof item.title !== "string")
              missing.push({ property: "title", type: "string" });
            if (typeof item.version !== "string")
              missing.push({ property: "version", type: "string" });
            if (item.description && typeof item.description !== "string")
              missing.push({ property: "description", type: "string" });
            if (item.author && typeof item.author !== "string")
              missing.push({ property: "author", type: "string" });
            if (typeof item.added !== "number")
              missing.push({ property: "added", type: "number" });
            if (item.prenotice && typeof item.prenotice !== "boolean")
              missing.push({ property: "prenotice", type: "boolean" });
            if (item.previews) {
              if (!(item.previews instanceof Array))
                missing.push({ property: "previews", type: "Array" });
              else
                for (let i = 0; i < item.previews.length; ++i) {
                  if (typeof item.previews[i] !== "string")
                    missing.push({ property: `previews[${i}]`, type: "string" });
                  else {
                    const extension = path.extname(item.previews[i]);
                    if (
                      extension !== ".png" &&
                      extension !== ".jpg" &&
                      extension !== ".jpeg" &&
                      extension !== ".mp4"
                    )
                      missing.push({
                        property: `previews[${i}]`,
                        type: ".png | .jpg | .jpeg | .mp4",
                      });
                  }
                }
            }
            switch (category) {
              case "games":
                if (
                  Object.prototype.toString.call(item.game) !==
                  "[object Object]"
                ) {
                  missing.push({ property: "game", type: "Object" });
                  break;
                }
                if (
                  Object.prototype.toString.call(item.game.file) !==
                  "[object Object]"
                )
                  missing.push({
                    property: "game.file",
                    type: "Object",
                  });
                else
                  for (const architecture in item.game.file)
                    if (typeof item.game.file[architecture] !== "string")
                      missing.push({
                        property: `game.file.${architecture}`,
                        type: "string",
                      });
                if (item.game.offline && typeof item.game.offline !== "boolean")
                  missing.push({ property: "game.offline", type: "boolean" });
                if (
                  !(
                    typeof item.game.difficulty === "number" &&
                    (item.game.difficulty === 0 ||
                      item.game.difficulty === 1 ||
                      item.game.difficulty === 2)
                  )
                )
                  missing.push({ property: "game.offline", type: "boolean" });
                break;
              case "movies":
                if (
                  Object.prototype.toString.call(item) !== "[object Object]"
                ) {
                  missing.push({ property: "movie", type: "Object" });
                  break;
                }
                if (typeof item.movie.file !== "string")
                  missing.push({ property: "movie.file", type: "string" });
                break;
              case "others":
                if (
                  Object.prototype.toString.call(item) !== "[object Object]"
                ) {
                  missing.push({ property: "other", type: "Object" });
                  break;
                }
                if (typeof item.other.file !== "string")
                  missing.push({ property: "other.file", type: "string" });
                break;
            }
            if (missing.length > 0) {
              missing.forEach((missing) =>
                logger.error(
                  utils.lang({
                    EN: `The property "${missing.property}" of ${missing.type} is missing in "${category}/${item.id}/info.json".`,
                    JP: `"${category}/${item.id}/info.json" 内に ${missing.type} のプロパティ "${missing.property}" が足りません。`,
                  })
                )
              );
              throw new Error(
                utils.lang({
                  EN: `"${category}/${item.id}/info.json" format is not valid.`,
                  JP: `"${category}/${item.id}/info.json" のフォーマットが正しくありません。`,
                })
              );
            }
            // return data
            logger.info(
              utils.lang({
                EN: `Fetched an item: ${JSON.stringify(item)}`,
                JP: `アイテム取得成功: ${JSON.stringify(item)}`,
              })
            );
            items.push(item);
          } catch (error) {
            logger.error(
              utils.lang({
                EN: "An error has occurred during loading an item.",
                JP: "アイテム読み込み失敗",
              })
            );
            logger.error(error);
            items.push({
              id: info.split("/")[info.split("/").length - 2],
              category,
              error,
            });
          }
        })
      );
      return items;
    }
  );

  electron.ipcMain.handle(
    "items.launch",
    async (_event, id: string, platform: string, args: string[]) => {
      logger.info(
        `Launching "${id}" as ${platform}... (args: ${JSON.stringify(args)})`
      );
      // launch program
    }
  );
}
