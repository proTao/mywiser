#! /bin/bash

function concentrateLine(){
    FILEPATH="$1"
    echo "handle $FILEPATH"
    ALLOW_END="。”！？…～"
    MAX_LENGTH=180
    oldline=""
    rm temp123
    for line in $(cat "$FILEPATH")
    do
        if [[ ${#oldline} -gt ${MAX_LENGTH} ]]
        then 
            echo "${oldline%。*}" >> temp123
            # echo before delete $oldline ${#oldline}
            oldline=${oldline##*。}
            # echo after delete $oldline
        fi
        oldline="${oldline}""$line"

        #if [[ ${ALLOW_END} =~ "${line: -1}" ]]
        #then # 正常结尾
        #    echo "$oldline" >> temp123
        #    oldline=""
        #else # 非正常结尾
        #    :
        #fi

    done
    echo "$oldline" >> temp123
}

concentrateLine $1 
