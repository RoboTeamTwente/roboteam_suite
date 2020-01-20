#!/bin/bash


echo "Repo: "
echo $2
echo "Branch: "
echo $1

if [[ ! -f "/opt/robotests/$2.sh" ]]; then
    echo "No such test found."
    exit 1
fi

bash "/opt/robotests/$2.sh" "$1"