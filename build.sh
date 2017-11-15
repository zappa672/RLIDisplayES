if ! [ -d 'build' ]; then
  mkdir build
fi
cd build

rm -rf *
/opt/Qt5.5.1/5.5/gcc_64/bin/qmake ../RLIDisplayES.pro -r -spec linux-g++ CONFIG+=debug
make
