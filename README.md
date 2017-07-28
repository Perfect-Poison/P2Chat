# P2Chat

## 一. 开发环境

**IDE**: 最好VS2015(或以上)

**数据库**：MySQL 5.7.16(x64)，或以上版本(x64)

**GUI**: 最好Qt5.8.0(x86)，或以上版本(x86)

Package文件夹下有，可以直接安装：
**MySql Connector**: mysql-connector-c-6.1.10-win32

## 二. 环境搭建

所有开发环境所需的软件安装完后，
1. 新建个文件夹用于放项目代码，双击进文件夹，然后右键**Git Bash Here**
2. 键入命令：**git clone git@gitlab.com:Perfect-Poison/P2Chat.git**，将项目代码从远程提取到本地
3. 安装**Package**文件夹内，**mysql-connector-c-6.1.10-win32**，请不要修改安装路径，默认安装即可。
4. VS2015或以上版本的环境搭建，这里统称vs，vs下需要安装对应的插件来支持Qt开发：<br>
	**vs2015**: http://download.qt.io/official_releases/vsaddin/qt-vs-tools-msvc2015-2.1.1.vsix <br>
	**vs2017**: http://download.qt.io/development_releases/vsaddin/qt-vsaddin-msvc2017-2.1.1-beta-10.03.2017.vsix
5. 用vs打开项目**(若是第一次打开该项目，会提示升级编译器，选择升级)**，上方会出现**Qt VS Tools -> Qt Options -> Add**，然后写上你**Qt5.8.0(x86)**，Path路径选择Qt安装目录下的**msvcxxx目录**，然后点击OK
6. 在vs右侧的**解决方案资源管理器**中，需要用到Qt的项目上右键**Qt Project Settings -> Properties -> Version（下拉，选择Qt5.8.0(x86)）**，点击OK

**至此，编译环境就搭好了**

**常用的git命令：**<br>
git pull origin master 更新本地代码<br>
git branch xxx 新建分支xxx<br>
git checkout xxx 切换到xxx分支<br>
git push origin xxx 将分支xxx的代码提交到远程仓库<br>
git status 查看当前有变化的文件<br>
git add . 将本地所有文件加入到索引列表(以后只要修改或者删除这些文件都会有记录)<br>
git commit -m "此次修改的内容"<br>
git还有很多命令，但工作中最常用到的只有这些命令，如果对于git内部运行机制或者命令感兴趣的话，可以看看Pro Git: http://git.oschina.net/progit/

## 三. 开源库使用

**Crypto++**：加密解密文件

**MySql++**: 数据库访问层