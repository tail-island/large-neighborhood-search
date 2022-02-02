Large Neighborhood Searchのサンプル実装です。

UnixライクなOSの場合。

~~~shell
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make
$ cd ..
$ ./t
~~~

Windows OSの場合（PowerShell）。

~~~powershell
> New-Item build -itemType Directory
> Set-Location .\build\
> cmake ..
> msbuild .\large-neighborhood-search.vcxproj /p:Configuration=Release
> Set-Location ..
> .\t.ps1
~~~

メモリ管理を効率化するためにBoostを使用しています（small_vectorサイコー！）。開発の際に使用したBoostのバージョンは1.78ですけど、たぶんもっと前のバージョンでも動作します。環境に合わせてCMakeLists.txtのBoostのバージョンを修正してください。
