/*!
 * ProjectName: fesgamelauncher
 * FileName: main.ts
 * Encoding: UTF-8
 * Author: TwoSquirrels
 * CreationDate: Aug 16, 2021
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

//import * as child from "child_process";
import * as electron from "electron";
import * as events from "events";
import * as fs from "fs-extra";
import * as path from "path";

import * as utils from "./utils";
import * as items from "./items";

/* constants */

const defaultConfig = {
  exhibition: false,
  log: "JP",
};
const config = (() => {
  const configPath = utils.userDataPath("config.json");
  if (fs.existsSync(configPath))
    return {
      ...defaultConfig,
      ...fs.readJsonSync(configPath),
    };
  fs.outputJson(configPath, defaultConfig, { spaces: 2 });
  return defaultConfig;
})();

/* main */

(async () => {
  await utils.init(config.log);

  // log4js setting
  const logger = utils.newLogger("ELECTRON");
  logger.info(
    utils.lang({
      EN: "Logger has finished initialization!",
      JP: "ロガー初期化完了",
    })
  );

  try {
    // This method will be called when Electron has finished
    // initialization and is ready to create browser windows.
    // Some APIs can only be used after this event occurs.
    await electron.app.whenReady();
    logger.info(
      utils.lang({
        EN: "Electron has finished initialization and is ready to create browser windows!",
        JP: "Electron初期化、ウィンドウ作成準備完了",
      })
    );

    // Register events
    electron.ipcMain.handle("utils.userDataPath", async (_event, ...paths: string[]) => utils.userDataPath(...paths))
    electron.ipcMain.handle("constants.config", async (_event) => config);
    items.register();

    // Create a window
    let mainWindow: electron.BrowserWindow | null;
    ((createWindow) => {
      createWindow();
      electron.app.on("activate", () => {
        // On macOS it's common to re-create a window in the app when the
        // dock icon is clicked and there are no other windows open.
        if (electron.BrowserWindow.getAllWindows().length === 0) createWindow();
      });
    })(() => {
      /* function createWindow() */
      // Create the browser window.
      logger.info(utils.lang({ EN: "Creating a window...", JP: "ウィンドウ作成開始" }));
      mainWindow = new electron.BrowserWindow({
        width: 1280,
        height: 768,
        minWidth: 360,
        minHeight: 480,
        autoHideMenuBar: true,
        kiosk: config.exhibition,
        fullscreen: config.exhibition,
        frame: !config.exhibition,
        webPreferences: {
          preload: path.join(__dirname, "electron.js"),
        },
      });
      logger.info(utils.lang({ EN: "Completed creating the windows!", JP: "ウィンドウ作成完了" }));
      // Register events
      mainWindow.webContents.on("console-message", (event, level, message, line, sourceId) => {
        const logger = utils.newLogger("WEB");
        const log = `${sourceId ?? "DevTools"}:${line}\n${message}`
        switch (level) {
          case 0:
            logger.trace(log);
            break;
          case 1:
            logger.info(log);
            break;
          case 2:
            logger.warn(log);
            break;
          case 3:
            logger.error(log);
            break;
        }
      });
      // and load the index.html of the app.
      logger.info(utils.lang({ EN: 'Loading "launcher.html"...', JP: '"launcher.html"読み込み開始' }));
      mainWindow.loadFile(path.join(__dirname, "site", "launcher.html"));
      logger.info(utils.lang({ EN: 'Completed loading "launcher.html"!', JP: '"launcher.html"読み込み完了' }));
    });

    // Quit when all windows are closed, except on macOS. There, it's common
    // for applications and their menu bar to stay active until the user quits
    // explicitly with Cmd + Q.
    await events.once(electron.app, "window-all-closed");
    if (process.platform !== "darwin") electron.app.quit();
    logger.info(utils.lang({ EN: "Window all closed." }));

    logger.info(
      utils.lang({
        EN: "Application has finished.",
        JP: "アプリケーション終了",
      })
    );
  } catch (err) {
    logger.error(
      utils.lang({ EN: "An error has occurred.", JP: "エラー発生" })
    );
    logger.error(err);
  }
})();
