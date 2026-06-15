# Resize Factor Decimal Point Fix

## Issue Fixed

**Problem:** Could not type decimal point (.) in the Resize Factor field

**Cause:** The edit control had the `ES_NUMBER` style flag, which only allows integers (no decimal points or negative signs)

## Solution

Removed the `ES_NUMBER` style from the Resize Factor edit control:

### Before
```cpp
g_hEditResizeFactor = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"2.00",
    WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,  // ? ES_NUMBER prevented decimals
    ...);
```

### After
```cpp
g_hEditResizeFactor = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"2.00",
    WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,  // ? ES_NUMBER removed
    ...);
```

## What Changed

**Now you can type:**
- ? Decimals: `2.5`, `1.75`, `0.5`
- ? Integers: `2`, `3`, `10`
- ? Multiple decimal places: `2.125`, `3.14159`

**Validation still works:**
- The application still validates the value when processing
- Invalid values (non-numeric, zero, negative) are caught with error messages
- Just more flexible input now

## Examples

### Valid Inputs (All work now)
```
2.0    ?
2.5    ?
1.75   ?
0.5    ?
0.25   ?
3.14   ?
10.0   ?
```

### Invalid Inputs (Caught during validation)
```
-1.0   ? (negative - error shown)
0      ? (zero - error shown)
abc    ? (not a number - error shown)
```

## Testing

Try typing these values in the Resize Factor field:
1. `2.5` - Should work now ?
2. `1.75` - Should work now ?
3. `0.5` - Should work now ?

## Technical Note

The `ES_NUMBER` style is useful for integer-only fields, but since we need decimal values for precise scaling (like 1.5x or 0.75x), we removed it. The validation in `ProcessFBXFiles()` still ensures only valid numeric values are processed.

**Build successful** - Restart the app and you can now type decimal points! ??
