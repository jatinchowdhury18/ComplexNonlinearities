#!/bin/sh

export PATH="/c/Program Files (x86)/Microsoft Visual Studio/2017/Community/MSBuild/15.0/Bin/":$PATH

for Dir in `find . -maxdepth 1 -type d`
do
    if [ -d $Dir/Plugin ]
        then
        name="${Dir:2}"

        echo "Building $name..."
        cd $Dir/Plugin/Builds/VisualStudio2017/
        MSBuild.exe -v:quiet -t:rebuild -p:Configuration=RELEASE -p:Platform=x64 $name.sln

        echo "Copying $name ..."
        cd ../../../../
        cp "$Dir/Plugin/Builds/VisualStudio2017/x64/Release/Standalone Plugin/$name.exe" Bin/$name/Win # standalone
        cp "$Dir/Plugin/Builds/VisualStudio2017/x64/Release/VST/$name.dll" Bin/$name/Win # VST
        cp "$Dir/Plugin/Builds/VisualStudio2017/x64/Release/VST3/$name.vst3" Bin/$name/Win # VST3
    fi
done
