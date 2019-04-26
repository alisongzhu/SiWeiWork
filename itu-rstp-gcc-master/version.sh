#!/bin/sh

commit_ts=`git log -1 --format="%ct"`

if uname | grep -q "Darwin"; then
	commit_time=`date -r$commit_ts +"%Y-%m-%d:%H:%M:%S"`
else
	commit_time=`date -d@$commit_ts +"%Y-%m-%d:%H:%M:%S"`
fi

commit_time=`date -r$commit_ts +"%Y-%m-%d:%H:%M:%S"`

#build_time=`date +"%Y-%m-%d %H:%M:%S"`

git_version=`git log -1 --format="%h"`

git_branch=`git symbolic-ref --short HEAD`

cd Inc

sed  s/MYVERSION/"$git_branch:$git_version:$commit_time"/g build_defs.h.in > build_defs.h

