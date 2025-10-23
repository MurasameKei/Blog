#!/usr/bin/env bash

# get sudo
echo "Validating user permissions"
sudo echo "Done"

# Compile and update the frontend
echo
echo "Compiling the website"
make clean-frontend
make
cp ./frontend/index.html ./build/frontend
cp -r ./frontend/style ./build/frontend
cp -r ./frontend/res ./build/frontend
cp -r ./frontend/blog ./build/frontend

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
./build/backend/BlogCore
