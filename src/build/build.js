const package_json = require(__dirname + "/package.json");

const ejs = require("ejs");
const fs = require("fs");
const uuid = require("uuid");

const template = fs.readFileSync(__dirname + "/src/resources/ejs/template.ejs", "utf8");

fs.writeFileSync(__dirname + "/build/index.html", ejs.render(template, {
  projectName: package_json.name,
  fileName: "index.html",
  encoding: "UTF-8",
  author: package_json.author,
  license: {
    name: package_json.license,
    url: package_json.licenseURL,
  },
  isArticle: false,
  keywords: [
    "自作ゲーム",
    "文化祭",
  ],
  description: "文化祭の為のゲームランチャー",
  color: "FFFFFF",
  url: "https://twosquirrels.github.io/FesGameLauncher/index.html",
  title: "FesGameLauncher",
  thumbnail: "/resources/img/thumbnails/index.jpg",
  randomUUID: uuid.v4(),
  twitter: "TwoSquirrels",
  icon: "/icon.ico",
  body: "<small>&copy; 2021 TwoSquirrels</small>",
}));

nw.Window.open("/build/index.html", {}, function(win) {});
