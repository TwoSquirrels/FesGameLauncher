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

type ItemOrError = {
  id: string;
  category: "games" | "movies" | "others";
  error?: Error;
};
type Item = ItemOrError & {
  title: string;
  version: string;
  description?: string;
  author?: string;
  added: number;
  prenotice?: boolean;
  error?: undefined;
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
    offline?: boolean;
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
  utils: {
    userDataPath: (...paths: string[]) => Promise<string>;
    checkInternet: (timeout?: number, host?: string) => Promise<boolean>;
  };
  constants: {
    config: Promise<{
      exhibition?: boolean;
      visibleNotOffline?: string;
    }>;
  };
  items: {
    get: (category: string) => Promise<ItemOrError[]>;
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
  if (!isElectron())
    history.replaceState("", "", `${extra.top()}launcher/${newPath}`);
  extra.path = newPath;
}

let items: ItemOrError[] = [];
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

const updateItems = async () => {
  // 読み込み中ならキャンセル
  document
    .querySelectorAll<Element>("main > .box > .loading")
    .forEach((box) => {
      if (!box.classList.contains("hide")) throw null;
    });
  console.log("Updating items...");
  // 読み込み中画面を重ねる
  document
    .querySelectorAll<Element>("main > .box > .loading")
    .forEach((box) => box.classList.remove("hide"));
  // 選択中タブの変更
  document
    .querySelectorAll<Element>("main > .box > .tabs > li")
    .forEach((tabLi) =>
      tabLi.classList.contains(tab)
        ? tabLi.classList.add("selected")
        : tabLi.classList.remove("selected")
    );
  // 読み込み
  items = isElectron()
    ? await electron.items.get(tab)
    : constants.page.data?.items?.[tab] ?? //[]
      {
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
            id: "真顔のアクション ver2.0",
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
            prenotice: true,
            movie: {
              file: "CoolestOfWorld.mp4",
            },
            category: "movies",
          },
        ],
        others: [],
      }[tab];
  // インターネット接続の確認
  const internet = isElectron() ? await electron.utils.checkInternet() : true;
  // itemsを更新する
  for (const itemList of document.querySelectorAll<Element>(
    "main > .box > .items"
  )) {
    itemList.innerHTML = "";
    itemList.classList.remove("games", "movies", "others");
    itemList.classList.add(tab);
    for (const itemOrError of items) {
      // アイテムを作る
      if (itemOrError.error) {
        // エラーアイテムの場合
        const error = document.createElement("div");
        error.classList.add("error");
        // notice
        const notice = document.createElement("div");
        notice.classList.add("notice");
        notice.innerHTML =
          '<i class="fas fa-exclamation-triangle"></i>&thinsp;アイテムの読み込みに失敗しました';
        error.appendChild(notice);
        // id
        const id = document.createElement("div");
        id.classList.add("id");
        id.innerHTML = "アイテムID:&thinsp;";
        // id/code
        const code = document.createElement("code");
        code.innerText = itemOrError.id;
        id.appendChild(code);
        error.appendChild(id);
        const li = document.createElement("li");
        li.appendChild(error);
        itemList.appendChild(li);
        console.error(
          `Failed to add "${itemOrError.id}" in ${tab} to the list.`
        );
      } else {
        // 正常に読み込めている場合
        const item: Item = itemOrError as Item;
        // フィルター
        if (isElectron() && item.category === "games") {
          const game = (item as Game).game;
          switch ((await electron.constants.config).visibleNotOffline) {
            case "never":
              if (!(game.offline ?? true)) continue;
              break;
            case "always":
              break;
            default:
              if (!(game.offline ?? true) && !internet) continue;
              break;
          }
        }
        const button = document.createElement("button");
        button.setAttribute("onclick", `switchItem("${item.id}")`);
        button.dataset.itemId = item.id;
        // 画像があればその画像をアイコンに、なければゲームパッドを
        try {
          const icon = document.createElement("img");
          console.log(
            isElectron()
              ? await electron.utils.userDataPath(
                  "items",
                  tab,
                  item.id,
                  "icon.png"
                )
              : `${extra.top()}${constants.page.top}items/${tab}/${
                  item.id
                }/icon.png`
          );
          icon.src = await resolveImage(
            isElectron()
              ? await electron.utils.userDataPath(
                  "items",
                  tab,
                  item.id,
                  "icon.png"
                )
              : `${extra.top()}${constants.page.top}items/${tab}/${
                  item.id
                }/icon.png`
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
          if (item.prenotice) ellipsis.classList.add("prenotice");
          ellipsis.innerText = item.title;
          div.appendChild(ellipsis);
        });
        appendInfo("author", (div) => {
          const ellipsis = document.createElement("div");
          ellipsis.classList.add("ellipsis");
          ellipsis.innerText = item.author ?? "制作者非公開";
          div.appendChild(ellipsis);
        });
        if (item.category === "games") {
          // ゲームのみ
          const game = (item as Game).game;
          appendInfo(
            "difficulty",
            (div) =>
              (div.innerHTML = `<i class="fas fa-${
                ["laugh", "smile", "angry"][game.difficulty]
              } fa-2x"></i><span class="wide-only">&thinsp;${
                ["かんたん", "ふつう", "むずかしい"][game.difficulty]
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
            (div.innerHTML =
              item.added === getKonnendo()
                ? '<span class="new">NEW!</span>'
                : `${item.added}<span class="wide-only">年度版</span>`)
        );
        // アイテムを追加する
        const li = document.createElement("li");
        li.appendChild(button);
        itemList.appendChild(li);
        console.log(`Successfully added "${item.id}" in ${tab} to the list.`);
      }
    }
  }
  // previewを消す
  eraseElement("main > .box > .preview > .item");
  unEraseElement("main > .box > .preview > .unselected");
  // プレビューをちょっと変える
  document
    .querySelectorAll<HTMLElement>("main > .box > .preview > .item > .text > .game")
    .forEach((gameOnly) =>
      tab === "games" ? unEraseElement(gameOnly) : eraseElement(gameOnly)
    );
  console.log("Completed updating items!");
  // 読み込み中画面を消す
  document
    .querySelectorAll<Element>("main > .box > .loading")
    .forEach((box) => box.classList.add("hide"));
};

const updatePreview = async () => {
  const itemOrError = items.find((item) => item.id === itemId);
  if (!itemOrError || itemOrError.error) return;
  const item = itemOrError as Item;
  // 選択中アイテムの変更
  document
    .querySelectorAll<HTMLElement>("main > .box > .items > li > button")
    .forEach((button) =>
      button.dataset.itemId === itemId
        ? button.classList.add("selected")
        : button.classList.remove("selected")
    );
  // アイコン
  document
    .querySelectorAll<Element>("main > .box > .preview > .item > .text > .icon")
    .forEach(async (icon) => {
      try {
        const img = document.createElement("img");
        img.src = await resolveImage(
          isElectron()
            ? await electron.utils.userDataPath(
                "items",
                tab,
                item.id,
                "icon.png"
              )
            : `${extra.top()}${constants.page.top}items/${tab}/${
                item.id
              }/icon.png`
        );
        icon.innerHTML = "";
        icon.appendChild(img);
      } catch (err) {
        icon.innerHTML = `<i class="fas fa-${
          { games: "gamepad", movies: "film", others: "splotch" }[item.category]
        } fa-lg"></i>`;
      }
    });
  // テキスト類
  document
    .querySelectorAll<HTMLElement>(
      "main > .box > .preview > .item > .text > .title"
    )
    .forEach((title) => (title.innerText = item.title));
  document
    .querySelectorAll<HTMLElement>(
      "main > .box > .preview > .item > .text > .version"
    )
    .forEach((version) => (version.innerText = `バージョン ${item.version}`));
  document
    .querySelectorAll<HTMLElement>(
      "main > .box > .preview > .item > .text > .author"
    )
    .forEach(
      (author) => (author.innerText = `制作者: ${item.author ?? "非公開"}`)
    );
  document
    .querySelectorAll<HTMLElement>(
      "main > .box > .preview > .item > .text > .added"
    )
    .forEach((added) => (added.innerText = `${item.added}年度に公開`));
  if (item.category === "games") {
    const game = (item as Game).game;
    document
      .querySelectorAll<HTMLElement>(
        "main > .box > .preview > .item > .text > .game > .difficulty"
      )
      .forEach(
        (difficulty) =>
          (difficulty.innerHTML = `<i class="fas fa-${
            ["laugh", "smile", "angry"][game.difficulty]
          } fa-lg"></i>&thinsp;${
            ["かんたん　", "ふつう　　", "むずかしい"][game.difficulty]
          }`)
      );
    document
      .querySelectorAll<HTMLElement>(
        "main > .box > .preview > .item > .text > .game > .offline"
      )
      .forEach(
        (offline) =>
          (offline.innerHTML = `<span style="color: ${
            game.offline ?? true ? "dimgray" : "royalblue"
          }; "><i class="fas fa-wifi fa-lg"></i></span>&thinsp;${
            game.offline ?? true ? "オフライン対応" : "ネット環境必須"
          }`)
      );
  }
  document
    .querySelectorAll<HTMLElement>(
      "main > .box > .preview > .item > .text > .description"
    )
    .forEach(
      (description) =>
        (description.innerHTML = decorationText(
          item.description ??
            "作者は相当この作品に自信があるようで、説明は用意されていません！？"
        ))
    );
  // TODO: メディア
  // TODO: 起動ボタン
  // プレビューの更新
  eraseElement("main > .box > .preview > .unselected");
  unEraseElement("main > .box > .preview > .item");
  onResize();
};;

function switchTab(tabName: "games" | "movies" | "others"): void {
  tab = tabName;
  changeExtra(`${tab}`);
  updateItems();
}

function switchItem(itemName: string): void {
  itemId = itemName;
  changeExtra(`${tab}/${itemId}`);
  updatePreview();
}

function onResize(): void {
  maximizingAdjustment();
  document.documentElement.style.setProperty(
    "--preview-height",
    `${document.querySelector("main > .box > .preview > .item")?.clientHeight ?? 0}px`
  );
}

// register events

window.onload = () => {
  console.log("The launcher page is now loaded.");
  onResize();
  updateItems();
  updatePreview();
  // 文化祭モードではないときはゲーム以外のタブの非表示を解除
  (async () => {
    if (!isElectron() || !(await electron.constants.config).exhibition)
      unEraseElement("main > .box > .tabs > li.erase")
  })();
};

window.onresize = onResize;
