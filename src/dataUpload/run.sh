#!/bin/bash
set -x 
env LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH java -classpath . cn.yunzhisheng.casr.EncodeContent
