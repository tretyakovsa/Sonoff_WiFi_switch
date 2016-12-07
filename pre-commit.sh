#!/bin/bash
gzip -vrkf dev_data/*.htm
gzip -vrkf dev_data/css/*.css
gzip -vrkf dev_data/js/*.js
gzip -vrkf dev_data/lang/lang.*.json

mv dev_data/*.htm.gz data/
mv dev_data/js/*.js.gz data/js
mv dev_data/css/*.css.gz data/css
mv dev_data/lang/*.json.gz data/lang
