/*!
 * ProjectName: fesgamelauncher
 * FileName: electron.ts
 * Encoding: UTF-8
 * Author: TwoSquirrels
 * CreationDate: Sep 20, 2021
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

////////////////////////////////////////////////////////////////////////////////
// imports
////////////////////////////////////////////////////////////////////////////////

import * as electron from "electron";

////////////////////////////////////////////////////////////////////////////////
// electron
////////////////////////////////////////////////////////////////////////////////

electron.contextBridge.exposeInMainWorld("electron", {
  utils: {
    userDataPath: async (...paths: string[]) => await electron.ipcRenderer.invoke("utils.userDataPath", ...paths),
    checkInternet: async (timeout?: number, host?: string) => await electron.ipcRenderer.invoke("utils.checkInternet", timeout, host),
  },
  constants: {
    config: electron.ipcRenderer.invoke("constants.config"),
    platform: electron.ipcRenderer.invoke("constants.platform"),
  },
  items: {
    get: async (category: string) =>
      await electron.ipcRenderer.invoke("items.get", category),
    launch: async (id: string, platform: string, args?: string[]) =>
      await electron.ipcRenderer.invoke(
        "items.launch",
        id,
        platform,
        args ?? []
      ),
  },
});
