#! /bin/bash
if [ $# -eq 1 ]; then
	javac -classpath ".:lib/*" -d ./build -verbose ./com/threedsystems/tws/*.java
	jar cvf ./jar/tws_sdk_$1.jar ./com/* ./lib/*.jar
else
	echo "usage: build [version]"
	echo "for example, $ build 0.4"
fi
