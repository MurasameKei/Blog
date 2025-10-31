#!/usr/bin/env bash

# get sudo
echo "Validating user permissions"
sudo echo "Done"

# Compile and update the backend
echo
echo "Compiling the backend"
make

# Compile and update the frontend
echo
echo "Compiling the frontend"
dir="./frontend/scripts"
targets="$dir/filtering.ts $dir/comments.ts $dir/topbar.ts"
echo $targets
#npx esbuild $targets --bundle --platform=node --target=ES2020 --minify --format=esm --splitting --outdir=./build/frontend/scripts/
npx esbuild $targets --bundle --platform=node --target=ES2020 --format=esm --splitting --outdir=./build/frontend/scripts/

# Copying the website data to a build folder
echo
echo "Copying website to a build folder"
cp ./frontend/index.html ./build/frontend
cp -r ./frontend/style ./build/frontend
cp -r ./frontend/res ./build/frontend
cp -r ./frontend/blog ./build/frontend

# Create Database
psql -h /run/postgresql -U postgres -d Blog -f ./database/blog.sql

# Sync build files to server folder
echo
echo "Syncing the server files"
sudo rm -rf /srv/http/Blog/
sudo cp -r ./build/frontend/ /srv/http/Blog/
sudo cp -f ./nginx/nginx.conf /etc/nginx/nginx.conf

# Reload the server
echo
echo "Reloading server configurations"
sudo systemctl restart nginx.service

# Starting the backend
echo
echo "Backend Start"
#valgrind --tool=memcheck ./build/backend/BlogCore
sudo ./build/backend/BlogCore
