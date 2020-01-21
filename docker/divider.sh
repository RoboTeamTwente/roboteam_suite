#!/bin/bash


# If you want to add a repo, you simply create a file called {repo_name}.sh in tests
# This should just contain the commands to run sequentially for the repo
# Should start with:
#
# cd /opt/roboteam/roboteam_suite
# git submodule foreach git checkout master
# git submodule foreach git pull
# cd {repo_name}
# git checkout "$1"


echo "Repo: "
echo $2
echo "Branch: "
echo $1

export CC="/usr/bin/clang"
export CCX="/usr/bin/clang++"

# Checks if the test for the repo exists
if [[ ! -f "/opt/robotests/$2.sh" ]]; then
    echo "No such test found."
    exit 1
fi

# Runs the test for the repo
bash "/opt/robotests/$2.sh" "$1"