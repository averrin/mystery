export GDK_BACKEND=x11
rm -rf ./src-tauri/mystery/include/rust
rm -rf ./src-tauri/mystery/include/src
pnpm tauri dev
