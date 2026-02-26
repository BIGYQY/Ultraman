## Ultraman
## 保存空文件夹
- 必须要用到.gitkeep，就是每个空目录下面都要有，走个形式
- 先查询有哪些空文件：
```bash
find . -type d -empty -not -path "./.git/*"
```
- 然后一键命令为其添加.gitkeep文件：
```bash
find . -type d -empty -not -path "./.git/*" -exec touch {}/.gitkeep \;
```
- 检查是否还有空文件夹：
```bash
ls -la */.gitkeep
```
- 之后每次有空文件夹，提交时都要来一遍这个命令，但是只要会bat就不用了......
