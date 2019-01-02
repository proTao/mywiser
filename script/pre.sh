#! /bin/bash
# 更好的自动编码转换工具：enca

:<<COMMENT
配置脚本环境变量: 
- SCRIPTDIR: 脚本文件夹绝对路径
- DOCDIR: 文档文件夹绝对路径
- WORKSPACE: 工作目录绝对路径
COMMENT
function setEnvironment() {
    SCRIPTDIR=$(dirname "$(realpath -e "${BASH_SOURCE}")")
    WORKSPACE=$(dirname "${SCRIPTDIR}")
    DOCDIR=$(realpath -e "$WORKSPACE"/doc)
    echo "WORKSPACE: " "$WORKSPACE"
    echo "SCRIPTDIR: " "$SCRIPTDIR"
    echo "DOCDIR: " "$DOCDIR"
}



:<<COMMENT
description: 把文件名中带书名号的文件重命名为书名号的内容
param1[in] FILEPATH: 需要被处理的文件路径 
return: None
state: None
COMMENT
function renameFile() {
    FILEPATH=$1
    DIRNAME=$(dirname "${FILEPATH}")
    FILENAME=$(basename "${FILEPATH}")
    FILESUBFIX=${FILENAME##*.}

    # 使用环视
    NEWNAME=$(ls "$FILEPATH" | grep -P "(?<=《).*(?=》)" -o).$FILESUBFIX
    if [[ $? -eq 0 ]]
    then
        mv "$FILEPATH" "$DIRNAME"/"$NEWNAME"
    fi

}

:<<COMMENT
description: 将GBK编码文件转换为UTF8编码，并替换原文件
param1[in] FILEPATH: 需要被处理的文件路径 
param2[in] SOURCECODING: 源文件的编码
return: None
state code: 
COMMENT
function toUTF8() {
    FILEPATH=$1
    SOURCECODING=$2
    DIRNAME=$(dirname "${FILEPATH}")
    FILENAME=$(basename "${FILEPATH}")
    TEMPFILENAME="temp"
    iconv -c -f "${SOURCECODING}" -t utf-8 "${FILEPATH}" > "${DIRNAME}"/"${TEMPFILENAME}"
    mv "${DIRNAME}"/"${TEMPFILENAME}" "${FILEPATH}"
}

:<<COMMENT
description: 检测目标文件是否属于需要被转换的编码
param1[in] FILEPATH: 需要被处理的文件路径
return: iconv可以接受的源格式
state code: 表示是否需要转换或加入日志：（2：无需转换但加入日志；0：无需转换；1：需要转换）
COMMENT
function getEncoding(){
    FILEPATH=$1
    FILEINFO=$(file "${FILEPATH}")

    # 检测是否是UTF-16，不区分大小端
    ret=$(echo "${FILEINFO}" | grep 'UTF-16')
    if [[ $? -eq 0 ]]
    then
        echo UTF-16
        return 1
    fi

    # 检测是否是GBK
    # Non-ISO就认为是GBK
    # 实际来看一些GBK编码中有乱码会被file命令识别成ISO-8859
    ret=$(echo "${FILEINFO}" | grep -P 'ISO-8859|Non-ISO')
    if [[ $? -eq 0 ]]
    then
        echo GBK
        return 1
    fi

    # 检测是否是无需转换的类型
    # 'ASCII' 左右要有空格
    ret=$(echo "${FILEINFO}" | grep -P 'UTF-8| ASCII ')
    if [[ $? -eq 0 ]]
    then
        echo GOOD
        return 0
    fi

    # 其他未处理的文本文件
    # WARNING
    ret=$(echo "${FILEINFO}" | grep 'text')
    if [[ $? -eq 0 ]]
    then
        echo UNKNOWN TEXT
        return 2
    fi

    echo OTHER
    return 2
}

:<<COMMENT
description: 过滤掉指定字符，目前是仅过滤\r，还有重复的空格和换行（由于管道，此时已经没有\r了）
param1[in]: 被处理的文件的绝对路径
COMMENT
function filterDirty() {
    FILEPATH="$1"
    tr -d "\r" < "${FILEPATH}"| tr -s '\n ' > temp
    mv temp "${FILEPATH}"
}

:<<COMMENT
description：主函数，
return: None
state code: None
COMMENT
function main(){
    for f in ${DOCDIR}/*
    do
        SOURCECODING=$(getEncoding "${f}")
        STATECODE=$?
        if [[ $STATECODE -eq 0 ]]
        then
            :
            echo "${f}" "UTF8 ENCODING PASS"
        elif [[ $STATECODE -eq 1 ]]
        then
            echo "toUTF8" "${f}" "$SOURCECODING"
            toUTF8 "${f}" "$SOURCECODING"
        else
            echo "${f}" "${SOURCECODING}" >> "${SCRIPTDIR}"/log
        fi

        renameFile "${f}"
        filterDirty "${f}"

    done
}

setEnvironment
main
