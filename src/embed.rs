extern crate libc;

use libc::c_char;
use std::ffi::{CStr,CString};
use std::vec::{Vec};

#[no_mangle]
pub extern fn rs_int_in_int_out(input: i32) -> i32{
    input*2
}

#[no_mangle]
pub extern fn rs_string_in_string_out(s_raw: *const c_char, out: *mut c_char) -> usize {
    // take string from the input C string
    if s_raw.is_null() { return 0; }

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

#[no_mangle]
pub extern fn rs_numeric_array_in_numeric_array_out(src_raw: *const u32, dst: *mut u32, size: usize) -> usize {
    // Convert to borrowed pointers.
    let src = unsafe { std::slice::from_raw_parts(src_raw, size) };
    let mut res: Vec<u32> = Vec::with_capacity(10);

    res.push(42);
    for i in 0..size {
        res.push(src[i]*2);
    }
    let res_size = res.len();

    unsafe{ std::ptr::copy(res.as_ptr(), dst, res_size); };

    return res_size;
}


#[repr(C)]
pub struct SomeStruct {
    some_item: i32,
    another_item: i32,
    test: i32,
    float_item: f32
}

#[no_mangle]
pub extern fn rs_struct_out() -> SomeStruct {
    let sss = SomeStruct {
        some_item: 3,
        another_item: 4,
        test: 5,
        float_item: 0.7_f32
    };
    return sss;
}


#[repr(C)]
pub struct OtherStruct {
  int_setting: i32,
  float_setting: f32,
  bool_setting: bool
}

#[no_mangle]
pub extern fn rs_object_as_struct_in_bool_out(data: OtherStruct) -> bool {
    println!("{}, {}, {}",data.int_setting, data.float_setting, data.bool_setting);
    true
}
