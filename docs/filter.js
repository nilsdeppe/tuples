
window.onload = function(){

    // Hide type alias RHS that refers to a metafunction result in a (*_)detail namespace
    $("body").children().
    find("td.memItemRight, td.memTemplItemRight, td.memname").each(function () {
        $(this).html( $(this).html().replace(/( =[ ]+)[typedef ]*typename [A-Za-z0-9]*[_]?detail[s]?::[A-Za-z0-9\-_\.&;<> /;:\"=,#]+::[A-Za-z0-9_]+/g,
            "$1implementation defined") );
    });
    
    // Hide type alias RHS that refers to a type in a (*_)detail namespace
    // Also remove ... = decltype(something(_detail)::)
    $("body").children().find("td.memItemRight, td.memTemplItemRight, td.memname").each(function () {
        $(this).html( $(this).html().replace(/( =[ ]+)[typedef ]*[A-Za-z0-9\(]*[_]?detail[s]?::[A-Za-z0-9\-_\.&;<> /;:\"=,#\(\)]+/g,
            "$1implementation defined") );
    });

    // Hide unnamed template parameters: , typename = void
    $("body").children().find("td.memItemRight, td.memTemplItemRight, div.memitem").each(function () {
        $(this).html( $(this).html().replace(/(template[&lt; ]+.*)(,[ ]*typename[ ]*=[ ]*void[ ]*)+/g,
            "$1") );
    });
    $("body").children().find("td.memItemRight, td.memTemplItemRight, div.memitem, div.title").each(function () {
        $(this).html( $(this).html().replace(/(&lt;[A-Za-z0-9 ,]+.*)(,[ ]*typename[ ]*&gt;)+/g,
            "$1&gt;") );
    });

    // Hide SFINAE in template parameters
    $("body").children().find("td.memTemplParams, div.memtemplate").each(function () {
        $(this).html( $(this).html().replace(/(template[A-Za-z0-9&;,\.=\(\) _]+)(,[ ]+typename[ ]+=[ ]+typename[ ]+std::enable_if.*::type)+/g,
            "$1") );
        
    });

    $("body").children().find("td.memTemplParams, div.memtemplate").each(function () {
        $(this).html( $(this).html().replace(/(template[A-Za-z0-9&;,\.=\(\) _]+)(,[ ]+typename[ ]+std::enable_if.*)+&gt;/g,
            "$1&gt;") );
        
    });

    $("body").children().find("td.memTemplParams, div.memtemplate").each(function () {
        $(this).html( $(this).html().replace(/bool Dummy = true/g,
            "") );
        
    });

    // Hide enable_if_t for SFINAE
    $("body").children().find("td.memTemplParams, div.memtemplate").each(function () {
        $(this).html( $(this).html()
            .replace(/(template[A-Za-z0-9&;,\.=\(\) _]+)(,[ ]+std::enable_if_t.*)+&gt;/g,
                "$1&gt;") );
        
    });

    // Italicize "implementation defined"
    $("body").children().find("div.memitem, td.memItemRight, td.memTemplItemRight").each(function () {
        $(this).html( $(this).html().replace(/implementation defined/g,
            "implementation defined".italics()) );
    });

    $("body").children().find("div.memitem, td.memItemRight, td.memTemplItemRight").each(function () {
        $(this).html( $(this).html().replace(/noexcept\(.*\)/g,
            "noexcept(if possible)".italics()) );
    });

    $("body").children().find("div.memitem, td.memItemRight, td.memTemplItemRight").each(function () {
        $(this).html( $(this).html().replace(/typename std::conditional&lt; is_copy_assignable::value, (<a class="el" href="classtuples_1_1tagged__tuple.html">tagged_tuple<\/a>), tuples_detail::no_such_type &gt;::type/g,
            "$1".italics()) );
    });

    $("body").children().find("div.memitem, td.memItemRight, td.memTemplItemRight").each(function () {
        $(this).html( $(this).html().replace(/typename std::conditional&lt; is_move_assignable::value, (<a class="el" href="classtuples_1_1tagged__tuple.html">tagged_tuple<\/a>), tuples_detail::no_such_type &gt;::type/g,
            "$1".italics()) );
    });

    
};
