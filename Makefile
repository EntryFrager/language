all:
	make -f frontend/MakeFrontend
	make -f backend/MakeBackend
	make -f backend_nasm/MakeBackendNasm
	make -f reverse_frontend/MakeReverseFrontend
	make -f middlend/MakeMiddlend
clean:
	rm -rf ./*.o
	rm -rf ./*.exe
	rm -rf ./tree/*.o
	rm -rf ./Include_file/*.o
	make -f frontend/MakeFrontend clean
	make -f backend/MakeBackend clean
	make -f backend_nasm/MakeBackendNasm clean
	make -f reverse_frontend/MakeReverseFrontend clean
	make -f middlend/MakeMiddlend clean

clean_debug:
	rm -rf ./debug/
