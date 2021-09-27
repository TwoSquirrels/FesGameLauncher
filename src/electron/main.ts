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
//import * as dateFormat from "dateformat";
import * as electron from "electron";
import * as fs from "fs-extra";
//import * as glob from "glob";
//import * as log4js from "log4js";
import * as path from "path";
//import * as util from "util";

import * as items from "./items";

/* constants */

const defaultConfig = {
  exhibition: false,
};
const config = (() => {
  const configPath = path.join(__dirname, "config.json");
  if (fs.existsSync(configPath)) return fs.readJsonSync(configPath);
  fs.outputJson(configPath, defaultConfig, { spaces: 2 });
  return {};
})();

/* utils */

/* electron */

function createWindow() {
  // Create the browser window.
  const mainWindow = new electron.BrowserWindow({
    width: 1280,
    height: 768,
    autoHideMenuBar: true,
    //kiosk: true,
    //fullscreen: true,
    //frame: false,
    webPreferences: {
      preload: path.join(__dirname, "electron.js"),
    },
  });
  // and load the index.html of the app.
  mainWindow.loadFile(path.join(__dirname, "site", "launcher.html"));
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
electron.app.whenReady().then(() => {
  // Register events
  electron.ipcMain.handle("config", async (event) => config);
  items.register();
  // Create a window
  createWindow();
  electron.app.on("activate", () => {
    // On macOS it's common to re-create a window in the app when the
    // dock icon is clicked and there are no other windows open.
    if (electron.BrowserWindow.getAllWindows().length === 0) createWindow();
  });
});

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
electron.app.on("window-all-closed", () => {
  if (process.platform !== "darwin") electron.app.quit();
});
