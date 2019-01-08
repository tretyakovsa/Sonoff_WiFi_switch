#!/bin/bash
cat dev_data/css/chartist.min.css dev_data/css/bootstrap.min.css dev_data/css/style.css > dev_data/css/build.css
cat dev_data/js/chartist.min.js dev_data/js/chart.js > dev_data/js/build.chart.js

#find ./dev_data3 -type f -iname '*.html' -exec sed -i -e 's|\?v\([0-9]\+\).\([0-9]\+\).\([0-9]\+\)|?ver=05.10.2017|g' '{}' \;
#find ./dev_data -iname "*.htm" -type f -exec sed -i 's/vvda$today/v07.06.2017/gi' {} \;

gzip -vrkf dev_data/*.htm
gzip -vrkf dev_data/css/*.css
gzip -vrkf dev_data/js/*.js
gzip -vrkf dev_data/*.json
gzip -vrkf dev_data/lang/lang.*.json
#gzip -vrkf dev_data/widgets/*.json

mv dev_data/*.htm.gz data/
mv dev_data/js/*.js.gz data/js
mv dev_data/css/*.css.gz data/css
mv dev_data/lang/*.json.gz data/lang
mv dev_data/*.json.gz data/
#mv dev_data/widgets/*.json data/widgets

rm dev_data/css/build.css
rm dev_data/js/build.chart.js

rm data/css/chartist.min.css.gz
rm data/css/bootstrap.min.css.gz
rm data/css/style.css.gz
rm data/js/chartist.min.js.gz
rm data/js/chart.js.gz
rm data/js/siema.min.js.gz

#test -f *.generic.bin && cp *.generic.bin build/build.generic_flash_size_1Mb.254Kb_`date '+%G.%m.%d'`.bin


#FILE_SUM=`md5sum Sonoff_WiFi_switch.ino.generic.bin`
#COPY_SUM=`md5sum ./build/$(ls -lt ./build | head -n2 |tail -n1 | awk '{print $9}')`
#if [ "$FILE_SUM" != "$COPY_SUM" ]
#then
#cp Sonoff_WiFi_switch.ino.generic.bin build/build.0x00000_flash_size_1Mb.256Kb_`date '+%G.%m.%d'`.bin
#fi
#cp /tmp/arduino_build_*/Sonoff_WiFi_switch.spiffs.bin ./build/spiffs.0xBB000_flash_size_1Mb.256Kb_`date '+%G.%m.%d'`.bin
