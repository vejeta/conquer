#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-or-later
# check-code-safety.sh - Quick safety audit for Conquer C codebase
# Copyright (C) 2025 Juan Manuel Méndez Rey (Vejeta)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
GPL_RELEASE="$PROJECT_ROOT/gpl-release"

RED='\033[0;31m'
YELLOW='\033[1;33m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo "================================================"
echo "Conquer Code Safety Check"
echo "================================================"
echo

# Check 1: Unsafe String Functions
echo -e "${YELLOW}[CHECK 1]${NC} Scanning for unsafe string functions..."
UNSAFE_FUNCS=$(grep -rn '\b\(gets\|sprintf\|strcpy\|strcat\)\s*(' "$GPL_RELEASE"/*.c 2>/dev/null | grep -v "strncpy\|snprintf\|strncat" || true)

if [ -n "$UNSAFE_FUNCS" ]; then
    echo -e "${RED}✗ FAIL${NC} - Found unsafe string functions:"
    echo "$UNSAFE_FUNCS" | head -20
    COUNT=$(echo "$UNSAFE_FUNCS" | wc -l)
    echo -e "${RED}Total: $COUNT occurrences${NC}"
    echo
else
    echo -e "${GREEN}✓ PASS${NC} - No unsafe string functions found"
    echo
fi

# Check 2: Unchecked malloc/calloc
echo -e "${YELLOW}[CHECK 2]${NC} Scanning for potentially unchecked memory allocations..."
cd "$GPL_RELEASE"
UNCHECKED_ALLOC=0

for file in *.c; do
    # Find malloc/calloc/realloc lines
    grep -n '\b\(malloc\|calloc\|realloc\)\s*(' "$file" 2>/dev/null | while IFS=: read -r line_num line_content; do
        # Check if next few lines have NULL check
        NEXT_LINES=$(sed -n "${line_num},$((line_num+3))p" "$file")
        if ! echo "$NEXT_LINES" | grep -q "NULL\|if.*!.*\|if.*==.*0"; then
            echo -e "${RED}✗${NC} $file:$line_num - Missing NULL check"
            echo "  $line_content"
            UNCHECKED_ALLOC=$((UNCHECKED_ALLOC+1))
        fi
    done
done

if [ $UNCHECKED_ALLOC -eq 0 ]; then
    echo -e "${GREEN}✓ PASS${NC} - All memory allocations appear to be checked"
else
    echo -e "${RED}✗ FAIL${NC} - Found $UNCHECKED_ALLOC potentially unchecked allocations"
fi
echo

# Check 3: Unchecked fopen
echo -e "${YELLOW}[CHECK 3]${NC} Scanning for potentially unchecked file operations..."
UNCHECKED_FOPEN=$(grep -rn '\bfopen\s*(' "$GPL_RELEASE"/*.c 2>/dev/null || true)
UNCHECKED_COUNT=$(echo "$UNCHECKED_FOPEN" | grep -v "^$" | wc -l)

if [ $UNCHECKED_COUNT -gt 0 ]; then
    echo -e "${YELLOW}⚠ WARNING${NC} - Found $UNCHECKED_COUNT fopen() calls"
    echo "  (Manual verification needed - checking for NULL checks)"
    echo "$UNCHECKED_FOPEN" | head -10
    echo
else
    echo -e "${GREEN}✓ PASS${NC} - No fopen calls found (or all checked)"
    echo
fi

# Check 4: Potential format string vulnerabilities
echo -e "${YELLOW}[CHECK 4]${NC} Scanning for potential format string vulnerabilities..."
FORMAT_VULN=$(grep -rn 'printf\s*(\s*[a-zA-Z_]' "$GPL_RELEASE"/*.c 2>/dev/null | grep -v '"%' || true)

if [ -n "$FORMAT_VULN" ]; then
    echo -e "${RED}✗ FAIL${NC} - Found potential format string vulnerabilities:"
    echo "$FORMAT_VULN" | head -10
    COUNT=$(echo "$FORMAT_VULN" | wc -l)
    echo -e "${RED}Total: $COUNT potential issues${NC}"
    echo
else
    echo -e "${GREEN}✓ PASS${NC} - No obvious format string vulnerabilities"
    echo
fi

# Check 5: Hardcoded buffer sizes
echo -e "${YELLOW}[CHECK 5]${NC} Scanning for common buffer size issues..."
BUFFER_ISSUES=$(grep -rn 'char.*\[80\]\|char.*\[256\]' "$GPL_RELEASE"/*.c 2>/dev/null | head -20 || true)

if [ -n "$BUFFER_ISSUES" ]; then
    echo -e "${YELLOW}⚠ INFO${NC} - Found hardcoded buffer sizes (not necessarily bad):"
    echo "$BUFFER_ISSUES" | head -10
    COUNT=$(echo "$BUFFER_ISSUES" | wc -l)
    echo -e "${YELLOW}Total: $COUNT occurrences${NC}"
    echo
else
    echo -e "${GREEN}✓ INFO${NC} - No common hardcoded buffer sizes found"
    echo
fi

# Check 6: Array bounds issues
echo -e "${YELLOW}[CHECK 6]${NC} Scanning for potential array indexing without bounds checks..."
ARRAY_ACCESS=$(grep -rn '\[[a-zA-Z_][a-zA-Z0-9_]*\]' "$GPL_RELEASE"/*.c 2>/dev/null | wc -l)

if [ $ARRAY_ACCESS -gt 0 ]; then
    echo -e "${YELLOW}⚠ INFO${NC} - Found $ARRAY_ACCESS array accesses"
    echo "  (Consider enabling -Warray-bounds compiler flag)"
    echo
fi

# Summary
echo "================================================"
echo "Summary"
echo "================================================"
echo
echo "This is a basic automated check. For comprehensive analysis:"
echo "  1. Run: make clean && make debug"
echo "  2. Run: valgrind --leak-check=full ./conquer"
echo "  3. Run: clang --analyze gpl-release/*.c"
echo "  4. Run: cppcheck --enable=all gpl-release/"
echo
echo "See IMPROVEMENTS.md for detailed recommendations"
echo "================================================"
