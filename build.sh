apt-get install -y libtinfo-dev unzip lrzsz locales

#init env
locale-gen en_US.UTF-8
update-locale LANG=en_US.UTF-8

#for vscode
if [ -d /root/clangd_20.1.8/ ] ; then
    echo "clangd already exist"
 else
    wget https://github.com/clangd/clangd/releases/download/20.1.8/clangd-linux-20.1.8.zip --no-check-certificate -O /root/clangd-linux-20.1.8.zip
    cd /root
    unzip /root/clangd-linux-20.1.8.zip
    echo "记得设置vsocde设置里面clangd的path"
fi

#for cpp_file build
if [ -d cfe-7.1.0.src ] ; then
    echo "cfe already exist"
else
    wget https://releases.llvm.org/7.1.0/cfe-7.1.0.src.tar.xz --no-check-certificate
    tar xf cfe-7.1.0.src.tar.xz
    cp -r cfe-7.1.0.src tools/clang
fi

# if [ -d clang-tools-extra-7.1.0.src ] ; then
#     echo "clang-tools-extra already exist"
# else
#     wget https://releases.llvm.org/7.1.0/clang-tools-extra-7.1.0.src.tar.xz --no-check-certificate
#     tar xf clang-tools-extra-7.1.0.src.tar.xz
#     cp -r clang-tools-extra-7.1.0.src tools/clang-tools-extra
# fi

mkdir -p build
cd build
cmake .. -DLIBCLANG_BUILD_STATIC=ON -DLLVM_ENABLE_EH=ON -DLLVM_ENABLE_RTTI=ON -DCMAKE_BUILD_TYPE=Debug -DPYTHON_EXECUTABLE=$(which python)
make -j16
make install
cd -