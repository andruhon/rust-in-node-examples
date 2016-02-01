extern crate libc;

use libc::c_char;
use std::ffi::{CStr,CString};


#[no_mangle]
pub extern fn rs_num_in_num_out(input: i32) -> i32{
    input*2
}

#[no_mangle]
pub extern fn rs_string_in_string_out(s_raw: *const c_char, out: *mut c_char) -> usize {
    // take string from the input C string
    let c_str: &CStr = unsafe { CStr::from_ptr(s_raw) };
    let buf: &[u8] = c_str.to_bytes();
    let str_slice: &str = std::str::from_utf8(buf).unwrap();
    let str_buf: String = str_slice.to_owned();

    //produce a new string
    let result = String::from(str_buf + " append from Rust");
    let len = result.len();

    //create C string for output
    let c_result = CString::new(result);

    //write string into out pointer passed by C++ addon
    unsafe{ std::ptr::copy(c_result.unwrap().as_ptr(), out, len); };

    // return result length
    return len;
}
