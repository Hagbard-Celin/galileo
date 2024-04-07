/* */

address GALILEO.1
options results
options failat 11

galileo front

if ~show('l','rexxsupport.library') then
	call addlib('rexxsupport.library',0,-30,0)

call openport('test-handler')

lister new 'c:'
handle=result
lister wait handle
lister set handle handler 'test-handler' quotes
lister set handle title 'test title!!!'
lister refresh handle full

/* install traps */
galileo addtrap copy 'test-handler'
galileo addtrap move 'test-handler'
galileo addtrap delete 'test-handler'
galileo addtrap rename 'test-handler'
galileo addtrap parent 'test-handler'
galileo addtrap read 'test-handler'
galileo addtrap doubleclick 'test-handler'

do while event ~= 'inactive'
	call waitpkt('test-handler')

	packet=getpkt('test-handler')
	event=getarg(packet,0)
	handle=getarg(packet,1)
	name=getarg(packet,2)
	user=getarg(packet,3)
	pathstr=getarg(packet,4)
	foostr=getarg(packet,5)
	boostr=getarg(packet,6)
	goostr=getarg(packet,7)

	say 0 event
	say 1 handle
	say 2 name
	say 3 user
	say 4 pathstr
	say 5 foostr
	say 6 boostr
	say 7 goostr

	call reply(packet,0)
end

/* remove all traps for my handler */
galileo remtrap '*' 'test-handler'
