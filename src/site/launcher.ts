/*!
 * ProjectName: fesgamelauncher
 * FileName: launcher.ts
 * Encoding: UTF-8
 * Author: TwoSquirrels
 * CreationDate: Sep 9, 2021
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

// references

/// <reference path="script.ts"/>

// contents

type Item = {
  id: string;
  title: string;
  version: string;
  description?: string;
  author?: string;
  added: number;
  category: "games" | "movies" | "others";
};
type Game = Item & {
  game: {
    file: {
      win32?: string;
      win64?: string;
      winarm?: string;
      osx64?: string;
      linux64?: string;
      linuxarm?: string;
      linuxarm64?: string;
      site?: string;
    };
    offline: boolean;
    difficulty: 0 | 1 | 2;
  };
};
type Movie = Item & {
  movie: {
    file: string;
  };
};
type Other = Item & {
  other: {
    site: string;
  };
};

declare const electron: {
  config: Promise<{
    exhibition?: boolean;
  }>;
  items: {
    get: (category: string) => Promise<(Game | Movie | Other)[]>;
    launch: (
      id: string,
      platform: string,
      arguments?: string[]
    ) => Promise<void>;
  };
};

// define functions

function isElectron(): boolean {
  return typeof electron !== "undefined";
}

function changeExtra(newPath: string): void {
  history.replaceState("", "", `${extra.top()}launcher/${newPath}`);
  extra.path = newPath;
}

const items: {
  games: Game[];
  movies: Movie[];
  others: Other[];
} = JSON.parse(
  JSON.stringify(
    constants.page.data?.items ?? {
      // { games: null, movies: null, others: null }
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
          category: "games",
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
          category: "games",
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
          category: "movies",
        },
      ],
      others: [],
    }
  )
);
let tab: "games" | "movies" | "others" = (() => {
  switch (extra.path.split("/")[0]) {
    case "games":
      return "games";
    case "movies":
      return "movies";
    case "others":
      return "others";
    default:
      return "games";
  }
})();
let itemId: string | null = extra.path.split("/")[1] ?? null;

const updateItems: () => Promise<void> = (() => {
  // 別のタブが読み込まれる前だったときに前のタブを反映させないためのID
  let nowId = -1;
  return async () => {
    const id = ++nowId;
    // 読み込み中画面を重ねる
    document
      .querySelectorAll<Element>("main > .box > .loading")
      .forEach((box) => box.classList.remove("hide"));
    // Electronの場合読み込み
    if (isElectron()) {
      switch (tab) {
        case "games":
          items.games = (await electron.items.get("games")) as Game[];
          break;
        case "movies":
          items.movies = (await electron.items.get("movies")) as Movie[];
          break;
        case "others":
          items.others = (await electron.items.get("others")) as Other[];
          break;
      }
    }
    //await new Promise((resolve) => setTimeout(resolve, 100));
    // 自身のIDが現在のIDと一致する場合のみ描画
    if (id === nowId) {
      // itemsを更新する
      for (const itemList of document.querySelectorAll<Element>(
        "main > .box > .items"
      )) {
        itemList.innerHTML = "";
        itemList.classList.remove("games", "movies", "others");
        itemList.classList.add(tab);
        for (const item of items[tab]) {
          // アイテムを作る
          const button = document.createElement("button");
          // 画像があればその画像をアイコンに、なければゲームパッドを
          try {
            const icon = document.createElement("img");
            icon.src = await resolveImage(
              `${isElectron() ? "../" : ""}items/${tab}/${item.id}/icon.png`
            );
            icon.classList.add("icon");
            button.appendChild(icon);
          } catch (err) {
            const icon = document.createElement("div");
            icon.innerHTML = `<i class="fas fa-${
              { games: "gamepad", movies: "film", others: "splotch" }[
                item.category
              ]
            } fa-lg"></i>`;
            icon.classList.add("icon");
            button.appendChild(icon);
          }
          // その他テキスト群
          const appendInfo = (
            infoType: string,
            fill: (div: HTMLDivElement) => void
          ): void => {
            const div = document.createElement("div");
            div.classList.add(infoType);
            fill(div);
            button.appendChild(div);
          };
          appendInfo("title", (div) => {
            const ellipsis = document.createElement("div");
            ellipsis.classList.add("ellipsis");
            ellipsis.innerText = item.title;
            div.appendChild(ellipsis);
          });
          appendInfo("author", (div) => {
            const ellipsis = document.createElement("div");
            ellipsis.classList.add("ellipsis");
            ellipsis.innerText = item.author ?? "製作者不明";
            div.appendChild(ellipsis);
          });
          if (item.category === "games") {
            // ゲームのみ
            const difficulty: 0 | 1 | 2 = (item as Game).game.difficulty;
            appendInfo(
              "difficulty",
              (div) =>
                (div.innerHTML = `<span class="${
                  ["easy", "normal", "hard"][difficulty]
                }"><i class="fas fa-${
                  ["laugh", "smile", "angry"][difficulty]
                } fa-2x"></i></span><span class="wide-only">&thinsp;${
                  ["かんたん", "ふつう", "むずかしい"][difficulty]
                }</span>`)
            );
            button.classList.add("is-game");
          }
          appendInfo("version", (div) => {
            div.innerHTML = '<span class="wide-only">バージョン</span>';
            div.appendChild(document.createTextNode(`${item.version}`));
          });
          appendInfo(
            "added",
            (div) =>
              (div.innerHTML = `${item.added}<span class="wide-only">年度版</span>`)
          );
          // アイテムを追加する
          const li = document.createElement("li");
          li.appendChild(button);
          itemList.appendChild(li);
        }
      }
      // previewを消す
      itemId = null;
      document
        .querySelectorAll<Element>("main > .box > .preview > .item")
        .forEach((preview) => preview.classList.add("hide"));
      document
        .querySelectorAll<Element>("main > .box > .preview > .unselected")
        .forEach((preview) => preview.classList.remove("hide"));
      // 読み込み中画面を消す
      document
        .querySelectorAll<Element>("main > .box > .loading")
        .forEach((box) => box.classList.add("hide"));
    }
  };
})();

function switchTab(tabName: "games" | "movies" | "others"): void {
  tab = tabName;
  changeExtra(`${tab}`);
  updateItems();
}

function switchItem(itemName: string): void {
  itemId = itemName;
  changeExtra(`${tab}/${itemId}`);
}

// register events

window.onload = (event) => {
  maximizingAdjustment();
  updateItems();
  // 文化祭モードではないときはゲーム以外のタブの非表示を解除
  (async () => {
    if (!isElectron() || !(await electron.config).exhibition)
      document
        .querySelectorAll<Element>("main > .box > .tabs > li.erase")
        .forEach((tab) => tab.classList.remove("erase"));
  })();
};

window.onresize = (event) => {
  maximizingAdjustment();
};
