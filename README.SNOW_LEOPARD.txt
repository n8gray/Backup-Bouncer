Important note for Snow Leopard users:

The version of rsync that Apple shipped with Snow Leopard (as of 10.6.1) has 
a bug that causes it to hang during the 90-fifo test in the backup bouncer 
test suite.  Don't run this test or use control-C to kill the test when this 
happens.  
