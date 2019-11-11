# Giới thiệu Project Simple Shell
Đồ án “chương trình Shell đơn giản” (Simple Shell) mô phỏng hoạt động
của một command-line interface (CLI) - giao diện dòng lệnh, trong đó người
dùng có thể nhập lệnh vào và thực thi lệnh. Chương trình Simple Shell hỗ trợ
một số lệnh cơ bản của hệ điều hành Ubuntu Linux như cd, cat, less, ls, pwd,
touch, mkdir, rm, find, ... Chương trình sẽ hỗ trợ input, output redirection cũng
như là pipes - một dạng giao tiếp (trao đổi, chuyển tiếp dữ liệu, output của tiến
trình này có thể trở thành input của tiến trình tiếp theo nó) giữa các câu lệnh /
tiến trình với nhau.
# Hướng dẫn sử dụng chương trình Shell đơn giản hoạt động trên Linux:

 1. Mở Terminal (Ctrl + Alt + T)

 2. Thay đổi đường dẫn đến thư mục chứa mã nguồn bằng câu lệnh: cd [đường dẫn]

	hoặc

 1. Đi vào thư mục _/source_ chứa file mã nguồn **shell.c**
 
 2. Click chuột phải vào vùng giữa màn hình Explorer, chọn _Open in Terminal_ 
 
 3. Gõ lệnh ``gcc shell.c -o shell`` để biên dịch (compile) mã nguồn
 
 4. Để chạy chương trình gõ lệnh: ``./shell`` 
 
 5. Chương trình đã chạy lên. Để kết thúc chương trình, gõ lệnh ``exit``
