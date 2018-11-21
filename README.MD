## 编译原理实验二 TINY+ IDE（到生成中间代码）

### 编译环境

cmake 3.12
make(mingw32-make on windows)
Qt 5.11.2
g++ gcc

cmake或Qt其他版本可修改CMakeLists.txt

#### 配置Qt

src/CMakeLists.txt中需指定Qt路径

> linux: set(CMAKE_PREFIX_PATH /qt_path/version/gcc_64)
> windows: set(CMAKE_PREFIX_PATH /qt_path/version/mingw53_32)

### 编译代码

```
git clone https://github.com/x-insane/tiny_compile.git
cd tiny_compile
cmake -DCMAKE_BUILD_TYPE=Release .. # windows需要加参数: -G "MinGW Makefiles"
make # windows: mingw32-make（最好使用Qt的mingw32-make的32位版本以免版本不兼容）

# windows导出Qt动态链接库（可选）
cd bin
windeployqt tiny_compile_run.exe --release
# 如果遇到无法定位程序入口错误，可能是libstdc++-6.dll版本不对，可以尝试替换为Qt目录下对应的版本
```
