#!/bin/sh

commit_ts=`git log -1 --format="%ct"`

if uname | grep -q "Darwin"; then
  commit_time=`date -r$commit_ts +"%Y-%m-%d:%H:%M:%S"`
else
  commit_time=`date -d@$commit_ts +"%Y-%m-%d:%H:%M:%S"`
fi

#commit_time=`date -r$commit_ts +"%Y-%m-%d:%H:%M:%S"`

build_time=`date +"%Y-%m-%d %H:%M:%S"`

git_version=`git log -1 --format="%h"`

git_branch=`git symbolic-ref --short HEAD`

#cd Core/Inc

#sed  s/GITBRANCH/"$git_branch"/g ./Core/Inc/build_defs.h.in > ./Core/Inc/build_defs.h

#sed  s/GITVERSION/"$git_version"/g ./Core/Inc/build_defs.h.in > ./Core/Inc/build_defs.h

sed -e s/GITBRANCH/"$git_branch"/g -e s/GITVERSION/"$git_version"/g -e s/BUILDTIMESTAMP/"$build_time"/g ./Core/Inc/build_defs.h.in > ./Core/Inc/build_defs.h


#sed  s/GITVERSION/"$git_branch:$git_version:$commit_time"/g ./Core/Inc/build_defs.h.in > ./Core/Inc/build_defs.h
