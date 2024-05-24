export GDK_BACKEND=x11
export LD_LIBRARY_PATH=/usr/local/lib/
rm -rf ./src-tauri/mystery/include/rust
rm -rf ./src-tauri/mystery/include/src
pnpm tauri dev
