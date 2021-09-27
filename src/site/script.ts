/*!
 * ProjectName: fesgamelauncher
 * FileName: script.ts
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

// types

declare const constants: {
  site: {
    name: string;
    message: string;
    url: string;
    libs: {
      css: string[];
      js: string[];
    };
    version: string;
  };
  page: {
    keywords: string[];
    description: string;
    author: string;
    color: string;
    type: string;
    title: string;
    twitter: string;
    libs: {
      css: string[];
      js: string[];
    };
    path: string;
    top: string;
    data?: any;
  };
};
declare const extra: { path: string; top: () => string };

// utils

function maximizingAdjustment() {
  document.documentElement.style.setProperty(
    "--width",
    `${document.documentElement.clientWidth}px`
  );
  document.documentElement.style.setProperty(
    "--height",
    `${document.documentElement.clientHeight}px`
  );
}

function resolveImage(path: string): Promise<string> {
  return new Promise<string>((resolve, reject) => {
    const image = new Image();
    image.onload = () => resolve(path);
    image.onerror = (err) => reject(err);
    image.src = path;
  });
}
