:- dynamic
	student/2,
	available_slots/1,
	room_capacity/2.


% count_fact( +Fact,-FactCount )
%
% Counts the facts in the knowledge base.

count_fact(Fact,FactCount) :-
	findall(_, Fact, FactList),
	length(FactList,FactCount).


% clear_knowledge_base.
%
% Clears all assertions from a KB and print a summary.

clear_knowledge_base :- 
	write("student/2: "),
	count_fact(student(_,_),StudentCount),
	writeln(StudentCount),
	write("available_slots/1: "),
	count_fact(available_slots(_),SlotCount),
	writeln(SlotCount),
	write("room_capacity/2: "),
	count_fact(room_capacity(_,_),RoomCount),
	write(RoomCount),
	retractall(student(_,_)),
	retractall(available_slots(_)),
	retractall(room_capacity(_,_)).


% all_students( −StudentList )
%
% Lists all students in a knowledge base.

all_students(StudentList) :-
	findall(StudentID, student(StudentID,_), StudentList).


% all_courses( -CourseList )
%
% Lists all unique courses.

all_courses(CourseList) :-
	findall(Course, ( student(_,List), member(Course,List) ), CourseListNotUnique),
	list_to_set(CourseListNotUnique,CourseList).


% student_count( +CourseID, −StudentCount )
%
% Gives student count of given course with CourseID.

student_count(CourseID, StudentCount) :-
	findall(Student, ( student(_,CourseList), member(CourseID,CourseList) ), StudentList),
	length(StudentList,StudentCount).


% common_students( +CourseID1, +CourseID2, −StudentCount )
%
% Gives the count of students who take both of the courses given with CourseID1 and CourseID2.

common_students(CourseID1, CourseID2, StudentCount) :-
	findall(Student, 
		(
			student(_,CourseList), 
			member(CourseID1,CourseList), 
			member(CourseID2,CourseList) 
		),
	 	StudentList),
	length(StudentList,StudentCount).


% final_plan( −FinalPlan )
%
% Calculates final exam time and locations without any conflict and report them
% as a list of lists that are in the following format: [CourseID, RoomID, Slot].
%
% Kind of a wrapper for add_courses predicate.
% Calculates CourseList, RoomList, SlotList and SlotRoomPairs not to calculate them
% again and again in add_courses predicate.

final_plan(FinalPlan) :-
	all_courses(CourseList),
	all_rooms(RoomList),
	available_slots(SlotList),
	pair_two_list(SlotList,RoomList,SlotRoomPair),
	add_courses(CourseList,SlotRoomPair,RoomList,FinalPlan,[]). % [] is calculated plan


% conflict_checker( +CurrentCoursePlan, +CalculatedPlan )
%
% Return false if plan contains two courses with same slot and there is students
% that take both of the courses 
% It pairs current course with calculated part of the plan and finds same slot pairs and
% checks conflict for these pairs

conflict_checker(CurrentCoursePlan, CalculatedPlan) :-
	pair_two_list(CurrentCoursePlan,CalculatedPlan,PlacePairs), %create all course pairs
	findall(Pair,
		(
			member(Pair,PlacePairs), 
			Pair=[[_,_,Slot],[_,_,Slot]]
		), 
		SameSlotPairs),   %select from course pairs the ones with same slot
	pair_conflict_checker(SameSlotPairs).  %checks if course pair have common students


% pair_conflict_checker( +CoursePairListWithSameSlot )
%
% Returns true if there is not any common student in the list of course pairs 

pair_conflict_checker([]).

pair_conflict_checker( [ [ [CourseID1,_,_],[CourseID2,_,_] ]|Tail ] ) :-
	common_students(CourseID1, CourseID2, 0),
	pair_conflict_checker(Tail).


% pair_two_list( +List1, +List2, -PairList )
%
% Pairs the elements of List1 with List2
% example: List1=[a,b] List2=[1,2]   PairList=[[a,1],[a,2],[b,1],[b,2]]
 
pair_two_list(List1,List2,PairList) :-
	findall([Elem1,Elem2], ( member(Elem1,List1), member(Elem2,List2) ), PairList).


% all_rooms( -RoomList )
%
% List all rooms in knowledge base

all_rooms(RoomList) :-
	findall(Room,room_capacity(Room,_),RoomList).


% add_courses( +CourseList, +SlotRoomPairs, +RoomList, -FinalPlan, +CalculatedPlan)
%
% Creates a list of [CourseID, RoomID, Slot] with the following properties:
% - student taking course <= room capacity
% - there should be all courses only once
% - for any course room and slot both can not be same
% - for two course with same slot, student taking them must be zero

% if there is not any course to add, the tail of FinalPlan is empty list
add_courses([],SlotRoomPair,RoomList,[],CalculatedPlan).
	
% if there is a course to add
add_courses([CourseID|Tail],SlotRoomPair,RoomList,[[CourseID,Room,Slot]|T],CalculatedPlan) :-
	member(Room,RoomList),    % find a room..
	excess_student_count(CourseID,Room,0),    % ..that can take all students of the course..
	member([Slot,Room],SlotRoomPair),    % ..and find a slot for this room
	conflict_checker([[CourseID,Room,Slot]],CalculatedPlan),
	append([[CourseID,Room,Slot]],CalculatedPlan,NewCalculatedPlan),
	delete(SlotRoomPair,[Slot,Room],NewSlotRoomPair),   
	%delete slot room pair for current Course from SlotRoomPair
	add_courses(Tail,NewSlotRoomPair,RoomList,T,NewCalculatedPlan). 
	%add the next course to the plan



% excess_student_count( +CourseID, +Room, -ExcessStudentCount )
%
% Returns the count of the student that can not fit in the Room for the CourseID.
% If all students can fit in the Room, it returns zero.

excess_student_count(CourseID,Room,ExcessStudentCount) :-
	student_count(CourseID,StudentCount),
	room_capacity(Room,Capacity),
	StudentCount > Capacity,
	!,
	ExcessStudentCount is StudentCount - Capacity.

excess_student_count(CourseID,Room,ExcessStudentCount) :-
	ExcessStudentCount is 0.


% excess_student( +FinalPlan, -ExcessError )
%
% Calculates the count of the students who cannot fit into the room for the course
% for each course in the FinalPlan

excess_student([],ExcessError) :-
	ExcessError is 0.

excess_student([[CourseID,Room,_]|T], ExcessError) :-
	excess_student_count(CourseID,Room,ExcessStudentCount),
	excess_student(T,NewExcessError),
	ExcessError is NewExcessError + ExcessStudentCount.


% all_pairs( +List, -PairList )
%
% Calculates every two element combination of a list.
% Example: List=[a,b,c] ==> PairList=[[a, b], [a, c], [b, c]]

all_pairs(List,PairList) :-
	findall([X,Y], (member(X,List), member(Y,List), X @< Y ), PairList).


% conflict_student_count( +PairList, -Count )

conflict_student_count([],Count) :-
	Count is 0.

conflict_student_count([ [[CourseID1,_,_],[CourseID2,_,_]] |Tail],Count) :-
	common_students(CourseID1,CourseID2,StudentCount),
	conflict_student_count(Tail,NewCount),
	Count is NewCount + StudentCount.


% conflict_count( +FinalPlan, -ConflictError )

conflict_count(FinalPlan,ConflictError) :-
	all_pairs(FinalPlan,PlacePairs), %create all course pairs
	findall(Pair,
		(
			member(Pair,PlacePairs), 
			Pair=[[_,_,Slot],[_,_,Slot]]
		), 
		SameSlotPairs),   %select from course pairs the ones with same slot
	conflict_student_count(SameSlotPairs,ConflictError). %count error for each pair

% errors_for_plan( +FinalPlan, −ErrorCount )
%
% Find errors in given FinalPlan and report their ErrorCount
%
% What is Error:
% 1) For any two exams in same slot, error goes up by one for every student who 
%   takes both of them.
% 2) For any exam that has been assigned to a classroom with smaller capacity than 
%   total number of attendee of corresponding course, error goes up by the amount:
%   NumberOfAttendee - RoomCapacity

errors_for_plan(FinalPlan,ErrorCount) :-
	excess_student(FinalPlan,ExcessError), % error for 2)
	conflict_count(FinalPlan,ConflictError), % error for 1)
	ErrorCount is ExcessError + ConflictError.

