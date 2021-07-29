const func = async () => {
  return new Promise(resolve => {
    setTimeout(() => {
      1resolve("5秒経ちました");
    }, 5000);
  });
};
