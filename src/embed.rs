#![feature(const_fn)]
extern crate libc;

use libc::{c_char,c_int, c_float};
use std::ffi::{CStr,CString};
use std::vec::{Vec};

use std::{mem,thread};
use std::time::Duration;

use std::sync::{Mutex, Arc};


#[no_mangle]
pub extern fn rs_int_in_int_out(input: c_int) -> c_int{
    input*2
}

#[no_mangle]
pub extern fn rs_string_in_string_out(s_raw: *const c_char, out: *mut c_char) -> c_int {
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
    return len as c_int;
}

#[no_mangle]
pub extern fn rs_numeric_array_in_numeric_array_out(src_raw: *const c_int, dst: *mut c_int, size: c_int) -> c_int {
    // Convert to borrowed pointers.
    let src = unsafe { std::slice::from_raw_parts(src_raw, size as usize) };
    let mut res: Vec<c_int> = Vec::with_capacity(10);

    res.push(42);
    for i in 0..(size as usize) {
        res.push(src[i]*2);
    }
    let res_size = res.len();

    unsafe{ std::ptr::copy(res.as_ptr(), dst, res_size); };

    return res_size as c_int;
}


#[repr(C)]
pub struct SomeStruct {
    some_item: c_int,
    another_item: c_int,
    test: c_int,
    float_item: c_float
}

#[no_mangle]
pub extern fn rs_struct_out() -> SomeStruct {
    let sss = SomeStruct {
        some_item: 3 as c_int,
        another_item: 4 as c_int,
        test: 5 as c_int,
        float_item: 0.7_f32 as c_float
    };
    return sss;
}


#[repr(C)]
pub struct OtherStruct {
  int_setting: c_int,
  float_setting: c_float,
  bool_setting: bool
}

#[no_mangle]
pub extern fn rs_object_as_struct_in_bool_out(data: OtherStruct) -> bool {
    println!("{}, {}, {}",data.int_setting, data.float_setting, data.bool_setting);
    true
}

// #[no_mangle]
// pub extern fn rs_num_c_func_with_c_callback(num: c_int, callback: extern fn(c_int)) {
//     //Left here for the reference
//     let handle = thread::spawn(move || {
//         let holder = unsafe {(*MUT_STATE).clone()};
//         let mut mutint = holder.lock().unwrap();
//         *mutint += 1;
//         println!("mutint {}", *mutint);
//         thread::sleep(Duration::new(2, 0));
//         unsafe { callback(num*2); }
//     });
// }

// The code below works well asynchronously with threads managed by node/NAN in C++
// and mutable state / locks managed by Rust
static mut MUT_STATE: *const Arc<Mutex<c_int>> = std::ptr::null();

#[no_mangle]
pub  extern fn rs_set_initial_state(state: c_int) {
    unsafe {
        let singleton = Arc::new(Mutex::new(state));
        MUT_STATE = mem::transmute(Box::new(singleton));
    }
}


#[no_mangle]
pub extern fn rs_slow_func(num: c_int) -> c_int {
    //If we take the lock before this long 1 sec "computation"
    //it will cause functions to run sequentally because of locks
    thread::sleep(Duration::new(1, 0));
    let holder = unsafe {(*MUT_STATE).clone()};
    let mut mutint = holder.lock().unwrap();
    *mutint += 1;
    println!("rs_slow_func internal state {}", *mutint);
    let multiplier = *mutint;
    num * multiplier
}
