#![feature(libc)]
#![feature(cstr_to_str)]
#![feature(cstr_memory)]
extern crate libc;
use std::ffi::{CStr,CString};

#[no_mangle]
pub extern fn rs_num_in_num_out(input: i32) -> i32{
    input*2
}

#[no_mangle]
pub extern fn rs_string_in_string_out(s_raw: *mut libc::c_char) -> *const libc::c_char {
    let s = unsafe {
        CString::from_raw(s_raw).to_str().unwrap().to_string()
    };
    //return CString::new("data data data data").unwrap().as_ptr();
    return CString::new(s + " fromRust").unwrap().as_ptr();
}
