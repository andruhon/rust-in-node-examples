#![feature(alloc_system)]
#![feature(const_fn)]
extern crate libc;

use libc::{c_char, c_int, c_double};
use std::ffi::{CStr,CString};
use std::vec::{Vec};

use std::{mem,thread};
use std::time::Duration;

use std::sync::{Mutex, Arc};

// #[no_mangle]
// pub extern "C" fn rs_drop(ptr: *const c_void) {
//   // We might need this function to allow outer app to drop things from rust
//   // However as docs say https://doc.rust-lang.org/book/custom-allocators.html#default-allocator
//   // Rust using alloc_system for libs, so we can easily use free in C/C++ code
//   // There's also feature(alloc_system) in the head just in case
//   // Used to drop rust allocated pointers from C
//   mem::drop(ptr);
// }

// Take c string in and return c string back
// with leaking result string and allowing receiver to deallocate it when needed
// this aproach presumes that rust and receiver both using the same allocation mechanism
// see https://doc.rust-lang.org/book/custom-allocators.html#default-allocator
// and https://github.com/rust-lang/rust/issues/31956
#[no_mangle]
pub extern "C" fn rs_string_in_string_with_append_from_rust_back(s_raw: *const c_char) -> *mut c_char {
    // take string from the input C string
    if s_raw.is_null() { panic!(); }
    let s: String = unsafe {CStr::from_ptr(s_raw).to_string_lossy().into_owned()};

    //produce a new string
    let result = String::from(s + " append from Rust");

    //create C string for output
    let c_string = CString::new(result).unwrap();
    let ret: *mut c_char = unsafe {mem::transmute(c_string.as_ptr())};
    mem::forget(c_string); // To prevent deallocation by Rust
    ret
}

#[no_mangle]
pub extern "C" fn rs_int_in_int_out(input: c_int) -> c_int{
    input*2
}

// Take string and write back into mutable pointer allocated in C++/V8
// without need to leak this value by rust
// with this approach we're sure that it is allocated and de-allocated by the same code
#[no_mangle]
pub extern "C" fn rs_string_in_string_back_to_buffer(s_raw: *const c_char, out: *mut c_char) -> c_int {
    // take string from the input C string
    if s_raw.is_null() { return 0; }
    let s: String = unsafe {CStr::from_ptr(s_raw).to_string_lossy().into_owned()};

    //produce a new string
    let result = String::from(s + " append from Rust");
    let len = result.len();

    //create C string for output
    let c_result = CString::new(result);

    //write string into out pointer passed by C++ addon
    unsafe{ std::ptr::copy(c_result.unwrap().as_ptr(), out, len); };

    // return result length
    return len as c_int;
}

#[no_mangle]
pub extern "C" fn rs_numeric_array_in_numeric_array_out(src_raw: *const c_int, dst: *mut c_int, size: c_int) -> c_int {
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
    float_item: c_double
}

#[no_mangle]
pub extern "C" fn rs_struct_out() -> SomeStruct {
    let sss = SomeStruct {
        some_item: 3 as c_int,
        another_item: 4 as c_int,
        test: 5 as c_int,
        float_item: 0.7_f32 as c_double
    };
    return sss;
}


#[repr(C)]
pub struct OtherStruct {
  int_setting: c_int,
  float_setting: c_double,
  bool_setting: bool
}

#[no_mangle]
pub extern "C" fn rs_object_as_struct_in_bool_out(data: OtherStruct) -> bool {
    println!("{}, {}, {}",data.int_setting, data.float_setting, data.bool_setting);
    true
}

// #[no_mangle]
// pub extern "C" fn rs_num_c_func_with_c_callback(num: c_int, callback: extern "C" fn(c_int)) {
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
pub  extern "C" fn rs_set_initial_state(state: c_int) {
    unsafe {
        let singleton = Arc::new(Mutex::new(state));
        MUT_STATE = mem::transmute(Box::new(singleton));
    }
}


#[no_mangle]
pub extern "C" fn rs_slow_func(num: c_int) -> c_int {
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
