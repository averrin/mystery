// Prevents additional console window on Windows in release, DO NOT REMOVE!!
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]
mod common;



// use extism::{manifest::Wasm, Manifest, Plugin};


// use std::fs::{self};
// use std::sync::Mutex;
use std::{env};
// use tauri::State;

// use config::Config;
// extern crate custom_error;
// use custom_error::custom_error;

#[cxx::bridge]
mod ffi {
    extern "C++" {
        include!("src/common.rs.h");

        type City = crate::common::City;
    }

    unsafe extern "C++" {
        include!("mystery/include/facade.hpp");

        fn init(root: String, seed: i32) -> City;
    }
}

#[tauri::command]
fn init(seed: i32) -> ffi::City {
    let mut p = env::current_exe().unwrap();
    p.pop();
    let p = p.clone();
    let root = String::from(p.to_str().unwrap()).to_owned();

    ffi::init(root, seed)
}

fn main() {
    tauri::Builder::default()
        .plugin(tauri_plugin_window_state::Builder::default().build())
        .invoke_handler(tauri::generate_handler![init,])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
