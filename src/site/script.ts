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
    data: any;
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

function getKonnendo(): number {
  const now = new Date();
  now.setMonth(now.getMonth() - 3);
  return now.getFullYear();
}

function eraseElement(elements: string | HTMLElement | HTMLElement[]): void {
  if (elements instanceof HTMLElement) {
    elements.classList.add("erase");
    elements.style.setProperty("display", "none");
  } else if (elements instanceof Array) elements.forEach(eraseElement);
  else document.querySelectorAll<HTMLElement>(elements).forEach(eraseElement);
}

function unEraseElement(elements: string | HTMLElement | HTMLElement[]): void {
  if (elements instanceof HTMLElement) {
    elements.classList.remove("erase");
    elements.style.removeProperty("display");
  } else if (elements instanceof Array) elements.forEach(unEraseElement);
  else document.querySelectorAll<HTMLElement>(elements).forEach(unEraseElement);
}

/**
 * 文字を装飾します(XSS対策もします)  
 * **TODO**: 以下実装予定  
 * - `<fas:` または `<fab:` と `>` の間に Font Awesome のアイコン名を入れることでアイコンを挿入する事ができます。
 * - `<color:` と `>` の間にカラーコードまたはカラーネームを入れることでそれより後ろの文字に色をつけることができます。
 *   `<colorless>` で色を戻せます。
 * - `<bold>` より後ろは太字になり、 `<fine>` で戻せます。
 */
function decorationText(text: string): string {
  return text.replace(/</g, "&lt;").replace(/\n/, "<br />");
}

// initialization

// とりあえずeraseを修正
eraseElement(".erase");
