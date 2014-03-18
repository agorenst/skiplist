package body Linked_List is

-- we redeclare the head here, just like in C++
procedure Insert (Container : in out List;
                  Item      : in     Element_Type)
is  -- open brace, more or less
begin -- define code
    if Container.Head = null then
        Container.Head := new Node'(Nxt => null, Data => Item);
    else
        declare -- i think this is necessary, to define a new value
            iterator : Node_Access := Container.Head;
        begin
            loop
                if iterator.Nxt = null then
                    iterator.Nxt := new Node'(Nxt => null, Data => Item);
                    exit;
                end if;
                iterator := iterator.Nxt;
            end loop;
        end;
    end if;
end Insert;


-- I'm not worrying about garbage collection yet.
-- Bug, seems to drop prefix before element.
-- Looking at http://globalengineer.wordpress.com/2009/03/27/ada-for-cc-developers-linked-list/ for reference
procedure Delete (Container : in out List;
                  Item      : in     Element_Type)
is
    -- we define these veariables explicitly at the top.
    P : Node_Access := null; -- predecessory to n
    N : Node_Access := Container.Head; -- our iterator
begin
    while N /= null loop
        if N.data = Item then
            if P = null then -- delete the first element!
                Container.Head := N.nxt;
            else -- the usual case
                P.nxt := N.nxt;
            end if;
            exit;
        else
            -- update our state.
            P := N;
            N := N.nxt;
        end if;
    end loop;
end Delete;

-- cool, this code could probably be made neater, but hey.
function "=" (Left, Right : List) return Boolean
is
    N : Node_Access := Left.Head;
    M : Node_Access := Right.Head;
begin
    loop
        if N /= null and M /= null then
            if N.data = M.data then
                N := N.nxt;
                M := M.nxt;
            else
                return false;
            end if;
        else
            if M = N then -- both are null
                return true;
            else
                return false;
            end if;
        end if;
    end loop;
end "=";


-- similar to above.
function Contains (Container : List;
                   Item      : Element_Type) return Boolean
is
    N : Node_Access := Container.Head;
begin
    loop
        if N = null then
            return False;
        else
            if N.data = Item then
                return True;
            else
                N := N.nxt;
            end if;
        end if;
    end loop;
end Contains;


end Linked_List;
