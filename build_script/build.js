(async () => {

  const path = require("path");
  const repsep = pathstr => pathstr.replace(/[/\\]/g, path.sep);

  const projectDir = process.cwd();
  const package_json = require(repsep(`${projectDir}/package.json`));

  const ejs = require("ejs");
  const fs = require("fs-extra");
  const uuid = require("uuid");

  const template =
    fs.readFileSync(repsep(`${projectDir}/src/main/ejs/template.ejs`), "utf8");

  fs.removeSync(repsep(`${projectDir}/build`));
  fs.mkdirsSync(repsep(`${projectDir}/build/nw`));
  fs.mkdirsSync(repsep(`${projectDir}/build/web`));
  fs.copySync(repsep(`${projectDir}/src/main`), repsep(`${projectDir}/build/nw`));
  fs.copySync(repsep(`${projectDir}/src/resources`), repsep(`${projectDir}/build/nw/resources`));
  fs.writeFileSync(
    repsep(`${projectDir}/build/nw/package.json`),
    JSON.stringify({
      name: package_json.name,
      version: package_json.version,
      description: package_json.description,
      main: "./node/main.js",
      repository: package_json.repository,
      author: package_json.author,
      license: package_json.license,
      licenseURL: package_json.licenseURL,
      bugs: package_json.bugs,
      homepage: package_json.homepage,
      window: package_json.window,
    }, undefined, 2)
  );
  fs.writeFileSync(
    repsep(`${projectDir}/build/nw/index.html`),
    ejs.render(template, {
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
    })
  );

})();
