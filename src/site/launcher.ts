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
  previews?: string[];
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

type Platform =
  | "win32"
  | "win64"
  | "winarm"
  | "osx64"
  | "linux64"
  | "linuxarm"
  | "linuxarm64";

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
    platform: Promise<Platform | "">;
  };
  items: {
    get: (category: string) => Promise<ItemOrError[]>;
    launch: (
      id: string,
      platform: Platform,
      args?: string[]
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

/**
 * 最適なプラットフォームを取得します。
 * @param platform 確かめるプラットフォーム名
 * @param supportedPlatforms 対応してるプラットフォーム名の配列
 * @returns 対応していたら最適なプラットフォーム、していなかったら空文字列 `""`
 */
function getBestPlatform(
  platform: Platform | "",
  supportedPlatforms: (Platform | "site")[]
): Platform | "site" | "" {
  // ちょうどいいやつはありますか？
  if (platform !== "" && supportedPlatforms.includes(platform)) return platform;
  // あらないんですか。なら互換性があるやつを...
  else if (platform.startsWith("win") && supportedPlatforms.includes("win32"))
    return "win32";
  // これホントに互換性あるのかな？不安になってきた
  else if (platform === "linuxarm64" && supportedPlatforms.includes("linuxarm"))
    return "linuxarm";
  // これでもなければサイトだ
  else if (supportedPlatforms.includes("site")) return "site";
  // サイトすらないなら対応してませんね
  else return "";
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
let previews: string[] = [];
let previewPage: number = 0;

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
            previews: ["thumbnail.png"],
            game: {
              file: {
                win32: "TheActionOfLookSerious_v2.0.exe",
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
  // TODO: ソート
  // シャッフル
  items = items.sort(() => Math.random() - 0.5);
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
          // プラットフォーム
          if (
            getBestPlatform(
              await electron.constants.platform,
              Object.keys(game.file) as (Platform | "site")[]
            ) === ""
          )
            continue;
          // ネット
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
          icon.src = await resolveImage(
            isElectron()
              ? await electron.utils.userDataPath(
                  "items",
                  item.category,
                  item.id,
                  "icon.png"
                )
              : `${extra.top()}${constants.page.top}items/${item.category}/${
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
              } fa-2x"></i><span class="wide-only">${
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
    .querySelectorAll<HTMLElement>(
      "main > .box > .preview > .item > .text > .game"
    )
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
  // プレビューをいろいろ書き換え
  const preview = "main > .box > .preview > .item";
  // アイコン
  document
    .querySelectorAll<Element>(`${preview} > .text > .icon`)
    .forEach(async (icon) => {
      try {
        const img = document.createElement("img");
        img.src = await resolveImage(
          isElectron()
            ? await electron.utils.userDataPath(
                "items",
                item.category,
                item.id,
                "icon.png"
              )
            : `${extra.top()}${constants.page.top}items/${item.category}/${
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
    .querySelectorAll<HTMLElement>(`${preview} > .text > .title`)
    .forEach((title) => (title.innerText = item.title));
  document
    .querySelectorAll<HTMLElement>(`${preview} > .text > .version`)
    .forEach((version) => (version.innerText = `バージョン ${item.version}`));
  document
    .querySelectorAll<HTMLElement>(`${preview} > .text > .author`)
    .forEach(
      (author) => (author.innerText = `制作者: ${item.author ?? "非公開"}`)
    );
  document
    .querySelectorAll<HTMLElement>(`${preview} > .text > .added`)
    .forEach((added) => (added.innerText = `${item.added}年度に公開`));
  if (item.category === "games") {
    const game = (item as Game).game;
    document
      .querySelectorAll<HTMLElement>(`${preview} > .text > .game > .difficulty`)
      .forEach(
        (difficulty) =>
          (difficulty.innerHTML = `<i class="fas fa-${
            ["laugh", "smile", "angry"][game.difficulty]
          } fa-lg"></i>${
            ["かんたん", "ふつう", "むずかしい"][game.difficulty]
          }`)
      );
    document
      .querySelectorAll<HTMLElement>(`${preview} > .text > .game > .offline`)
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
    .querySelectorAll<HTMLElement>(`${preview} > .text > .description`)
    .forEach(
      (description) =>
        (description.innerHTML = decorationText(
          item.description ??
            "作者は相当この作品に自信があるようで、説明は用意されていません！？"
        ))
    );
  // メディア
  previews = item.previews ?? [];
  slidePreview();
  // 起動ボタン
  (async () => {
    if (item.prenotice ?? false) {
      eraseElement(
        `${preview} > .media-and-buttons > .launch-buttons > .buttons`
      );
      unEraseElement(
        `${preview} > .media-and-buttons > .launch-buttons > .prenotice`
      );
    } else {
      unEraseElement(
        `${preview} > .media-and-buttons > .launch-buttons > .buttons`
      );
      eraseElement(
        `${preview} > .media-and-buttons > .launch-buttons > .prenotice`
      );
      const files = new Map<Platform | "site" | "movie", string>();
      switch (item.category) {
        case "games":
          const game = (item as Game).game;
          if (isElectron()) {
            // Electronの場合は最適なものがあるはず
            const bestPlatform = getBestPlatform(
              await electron.constants.platform,
              Object.keys(game.file) as (Platform | "site")[]
            );
            if (bestPlatform !== "")
              files.set(bestPlatform, game.file[bestPlatform] ?? "");
          } else {
            // サイトの場合はダウンロードボタンになるので全部
            if (game.file.site) files.set("site", game.file.site);
            for (const platform of Object.keys(game.file) as (
              | Platform
              | "site"
            )[])
              if (platform !== "site")
                files.set(platform, game.file[platform] ?? "");
          }
          break;
        case "movies":
          files.set("movie", (item as Movie).movie.file);
          break;
        case "others":
          files.set("site", (item as Other).other.site);
          break;
      }
      document
        .querySelectorAll<Element>(
          `${preview} > .media-and-buttons > .launch-buttons > .buttons`
        )
        .forEach((buttons) => {
          buttons.innerHTML = "";
          for (const [platform, file] of files) {
            // ボタンの追加
            const button = document.createElement("button");
            switch (item.category) {
              case "games":
                button.innerHTML = isElectron()
                  ? 'プレイ&thinsp;<i class="fas fa-play fa-lg"></i>'
                  : platform === "site"
                  ? 'Webでプレイ&thinsp;<i class="fas fa-play fa-lg"></i>'
                  : `${platform}版DL&thinsp;<i class="fas fa-download fa-lg"></i>`;
                break;
              case "movies":
                button.innerHTML =
                  '視聴&thinsp;<i class="far fa-play-circle fa-lg"></i>';
                break;
              case "others":
                button.innerHTML =
                  'ＧＯ&thinsp;<i class="fas fa-play fa-lg"></i>';
                break;
            }
            button.title = file;
            button.setAttribute(
              "onclick",
              `launch("${item.category}", ${JSON.stringify(
                item.id
              )}, ${JSON.stringify(file)}, "${platform}")`
            );
            buttons.appendChild(button);
          }
        });
    }
  })().catch(console.error);
  // プレビューの更新
  eraseElement("main > .box > .preview > .unselected");
  unEraseElement(preview);
  onResize();
};

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

/**
 * プレビューのページをスライド
 * @param move 相対的に何ページスライドするか(指定しなかった場合0にもどる)
 */
function slidePreview(move?: number): void {
  const itemOrError = items.find((item) => item.id === itemId);
  if (!itemOrError || itemOrError.error) return;
  const item = itemOrError as Item;
  const media = "main > .box > .preview > .item > .media-and-buttons > .media";
  if (previews.length === 0 || !previews.some((preview) => preview !== "")) {
    // プレビューがないとき
    eraseElement(`${media} > .content`);
    unEraseElement(`${media} > .no-content`);
  } else {
    // プレビューがあるとき
    unEraseElement(`${media} > .content`);
    eraseElement(`${media} > .no-content`);
    previewPage =
      move === undefined
        ? 0
        : (previewPage + move + previews.length) % previews.length;
    const preview = previews[previewPage];
    // 反映
    eraseElement(`${media} > .content > img`);
    eraseElement(`${media} > .content > video`);
    switch ((preview.match(/[^.]+$/) ?? [""])[0]) {
      case "png":
      case "jpg":
      case "jpeg":
        // 画像
        document
          .querySelectorAll<HTMLImageElement>(`${media} > .content > img`)
          .forEach(async (img) => {
            try {
              img.src = await resolveImage(
                isElectron()
                  ? await electron.utils.userDataPath(
                      "items",
                      item.category,
                      item.id,
                      "previews",
                      preview
                    )
                  : `${extra.top()}${constants.page.top}items/${
                      item.category
                    }/${item.id}/previews/${preview}?ver=${item.version}`
              );
              unEraseElement(img);
            } catch (err) {
              console.error("The preview image does not exist.");
              console.error(err);
              previews[previewPage] = "";
              slidePreview(move ?? 1);
            }
          });
        break;
      case "mp4":
        // 動画
        document
          .querySelectorAll<HTMLVideoElement>(`${media} > .content > video`)
          .forEach(async (video) => {
            video.src = isElectron()
              ? `file:///${await electron.utils.userDataPath(
                  "items",
                  item.category,
                  item.id,
                  "previews",
                  preview
                )}`
              : `${extra.top()}${constants.page.top}items/${item.category}/${
                  item.id
                }/previews/${preview}?ver=${item.version}`;
            unEraseElement(video);
          });
        break;
      default:
        // 画像/動画がないとわかっているとき
        slidePreview(move ?? 1);
        break;
    }
  }
}

async function launch(
  category: "games" | "movies" | "others",
  id: string,
  file: string,
  platform: Platform | "site" | "movie"
): Promise<void> {
  if (platform === "site" || platform === "movie") {
    // TODO: サイトと動画に対応させる
    alert("ごめんそれまだ未対応すぐ対応させるからまってて");
  } else if (isElectron()) {
    // 展示モードなら戻る
    if ((await electron.constants.config).exhibition) switchTab("games");
    // Electronは起動
    await electron.items.launch(id, platform);
  } else {
    // サイトはDL
    const a = document.createElement("a");
    a.href = `${extra.top()}${constants.page.top}items/games/${id}/files.zip`;
    a.download = `${id}.zip`;
    a.click();
  }
}

function onResize(): void {
  maximizingAdjustment();
  document.documentElement.style.setProperty(
    "--preview-height",
    `${
      document.querySelector("main > .box > .preview > .item")?.clientHeight ??
      0
    }px`
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
      unEraseElement("main > .box > .tabs > li.erase");
  })();
};

window.onresize = onResize;
