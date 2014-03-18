with Linked_List;
with Ada.Text_IO;
procedure driver is
    package Int_List is new Linked_List(Integer);
    list1 : Int_List.List := Int_List.Empty_List;
    list2 : Int_List.List := Int_List.Empty_List;
    are_eq : Boolean := false;
begin
    list1.Insert(1);
    list1.Insert(2);
    list1.Insert(3);
    list1.Insert(4);

    list2.Insert(1);
    list2.Insert(2);
    list2.Insert(3);
    list2.Insert(4);


    Ada.Text_IO.Put_Line(Boolean'Image(list1.Contains(1)));
    Ada.Text_IO.Put_Line(Boolean'Image(list2.Contains(4)));
    Ada.Text_IO.Put_Line(Boolean'Image(list1.Contains(2)));
    Ada.Text_IO.Put_Line(Boolean'Image(list1.Contains(3)));
    Ada.Text_IO.Put_Line(Boolean'Image(list1.Contains(4)));

    Ada.Text_IO.Put_Line(Boolean'Image(Int_List."="(list1,list2)));
    list2.Delete(3);
    Ada.Text_IO.Put_Line(Boolean'Image(list2.Contains(1)));
    Ada.Text_IO.Put_Line(Boolean'Image(list2.Contains(2)));
    Ada.Text_IO.Put_Line(Boolean'Image(list2.Contains(3)));
    Ada.Text_IO.Put_Line(Boolean'Image(list2.Contains(4)));
    Ada.Text_IO.Put_Line(Boolean'Image(Int_List."="(list1,list2)));

    
end driver;
