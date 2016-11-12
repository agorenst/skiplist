with Ordered_List;
with Ada.Text_IO;
procedure driver is
    package Int_List is new Ordered_List(Integer);
    list1, list2, list3, list4 : Int_List.List := Int_List.Empty_List;
    are_eq : Boolean := false;
begin
    list1.Insert(1);
    list1.Insert(2);
    list1.Insert(3);
    list1.Insert(4);

    list2.Insert(4);
    list2.Insert(3);
    list2.Insert(2);
    list2.Insert(1);

    list3.Insert(1);
    list3.Insert(2);
    list3.Insert(4);
    list3.Insert(3);

    list4.Insert(1);
    list4.Insert(3);
    list4.Insert(2);
    list4.Insert(4);

    Ada.Text_IO.Put_Line(Boolean'Image(Int_List."="(list1,list2)));
    Ada.Text_IO.Put_Line(Boolean'Image(Int_List."="(list3,list2)));
    Ada.Text_IO.Put_Line(Boolean'Image(Int_List."="(list4,list2)));
end driver;
