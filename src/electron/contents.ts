/*!
 * ProjectName: fesgamelauncher
 * FileName: contents.ts
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

/* module */

export function register() {
  electron.ipcMain.handle("contents.get", async (event, category: string) => {
    return {
      games: [
        {
          id: "ArcherStory",
          title: "アーチャー物語",
          version: "1.2",
          description: "スマホゲーム「アーチャー伝説」のパクリゲームです。",
          author: "２匹のりす",
          added: 2020,
          game: {
            file: {
              win32: "ArcherStory_v1.2.exe",
              win64: "ArcherStory_v1.2.exe",
              winarm: "ArcherStory_v1.2.exe",
            },
            offline: true,
            difficulty: 1,
          },
          icon: "data/stable/img/player/right.png",
          detail: "detail.png",
        },
        {
          id: "TheActionOfLookSerious",
          title: "真顔のアクション Version 2",
          version: "2.0",
          description:
            "しょぼ○のアクションのパクリゲームです。\n１面は簡単ですが、２面からはトラップが大量にあります。",
          author: "２匹のりす",
          added: 2019,
          game: {
            file: {
              win32: "TheActionOfLookSerious_v2.0.exe",
              win64: "TheActionOfLookSerious_v2.0.exe",
              winarm: "TheActionOfLookSerious_v2.0.exe",
            },
            offline: true,
            difficulty: 2,
          },
          icon: "icon.png",
          detail: "screenshot.png",
        },
      ],
      movies: [
        {
          id: "CoolestOfWorld",
          title: "世界最高にかっこいい動画",
          version: "1.0.0",
          description:
            "この動画は世界最高にかっこいいです。\n異論は２匹のりすに言ってください。",
          author: "かわいい鶏",
          added: 2021,
          movie: {
            file: "CoolestOfWorld.mp4",
          },
          icon: "icon.png",
          detail: "preview.mp4",
        },
      ],
      others: [],
    }[category];
  });

  electron.ipcMain.handle(
    "contents.launch",
    (event, id: string, platform: string, args: string[]) => {
      // launch program
    }
  );
}
