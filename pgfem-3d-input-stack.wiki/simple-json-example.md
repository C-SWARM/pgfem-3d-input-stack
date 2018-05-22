```json
{ "#": "Each input file is a large object, so they start with { and end with }",
  
  "bool_val": true,
  "string_val": "string",
  "int_val": 7,
  "double_val": 7.5,
  "#": "NOTE: Numbers cannot start with a decimal; eg: .5 won't work",

  "#": "scientific notation can use either e or E",
  "exponent1": 7.5e8,
  "exponent2": 7.0E-8,
  "#": "NOTE: The e cannot directly follow a decimal; eg: 7.e8 won't work",

  "object":
  {
    "item1": 1.2,
    "item2": 5
  },

  "#": "elements in arrays don't use tags",
  "array1": [1,2,3,4],

  "#": "arrays and objects can be nested",
  "array2":
  [
    { "#": "First element in array2; array2[0]",
      "array_object_array1": [1,2,3],
      "array_object_array2": ["a","b","c"]
    },

    { "#": "Second element in array2; array2[1]",
      "array_object_array1": [4,5,6],
      "array_object_array2": ["d","e","f"]
    }

  ],  "#": "End of array2",

  "#": "arrays can also store elements of different data types",
  "array3": [1,"string",3.2,4.0e2],

"#": "End of file"
}